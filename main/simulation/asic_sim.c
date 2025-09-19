#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <esp_log.h>
#include <esp_random.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "asic.h"
#include "mining.h"
#include "asic_common.h"
#include "esp_timer.h"
#include "../tasks/hashrate_monitor_task.h"

static const char *TAG = "ASIC_SIM";

// Simulated state
static uint8_t simulated_chip_count = 0;
static float simulated_frequency = 400.0f; // MHz
static uint32_t simulated_version_mask = 0;
static int simulated_baud_rate = 115200;
static uint8_t simulated_job_id_counter = 0;

// Simulated mining results
static task_result simulated_result;
static bool has_pending_result = false;
static uint32_t last_result_time = 0;

static uint32_t simulated_total_counters[4] = {0};
static uint32_t simulated_domain_counters[4] = {0};
static uint32_t simulated_error_counters[4] = {0};
static uint32_t last_register_read_time_ms = 0;
static const uint64_t SIM_HASH_RATE_UNIT = 0x100000ULL;

/**
 * @brief Initialize the ASIC simulation
 */
uint8_t ASIC_init(GlobalState * GLOBAL_STATE) {
    ESP_LOGI(TAG, "ASIC SIMULATION: Initializing simulated ASIC miner");

    // Seed random number generator
    srand(esp_random());

    // Initialize the valid_jobs_lock mutex for simulation
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&GLOBAL_STATE->valid_jobs_lock, &attr);
    pthread_mutexattr_destroy(&attr);

    // Use configured ASIC count for consistency with hashrate_monitor allocations
    simulated_chip_count = GLOBAL_STATE->DEVICE_CONFIG.family.asic_count;
    simulated_frequency = GLOBAL_STATE->DEVICE_CONFIG.family.asic.default_frequency_mhz; // Default frequency, will be set by ASIC_set_frequency

    ESP_LOGI(TAG, "ASIC SIMULATION: Using configured %d ASIC chips", simulated_chip_count);
    ESP_LOGI(TAG, "ASIC SIMULATION: Initial frequency: %.1f MHz", simulated_frequency);

    return simulated_chip_count;
}

/**
 * @brief Process work results (simulate mining)
 */
task_result * ASIC_process_work(GlobalState * GLOBAL_STATE) {
    // Ensure arrays are allocated before processing
    if (GLOBAL_STATE->ASIC_TASK_MODULE.active_jobs == NULL || GLOBAL_STATE->valid_jobs == NULL) {
        ESP_LOGD(TAG, "ASIC SIMULATION: Arrays not allocated, cannot process work");
        return NULL;
    }

    uint32_t current_time = xTaskGetTickCount();

    // Use the SAME calculation as the real system for expected hashrate
    // This matches what the UI displays: frequency * small_core_count * asic_count / 1000
    double expected_hashrate_ghs = simulated_frequency
                                 * GLOBAL_STATE->DEVICE_CONFIG.family.asic.small_core_count
                                 * GLOBAL_STATE->DEVICE_CONFIG.family.asic_count
                                 / 1000.0;

    // Calculate expected results per second for mining simulation
    // hashrate = (difficulty * 2^32) / time_per_result
    // time_per_result = (difficulty * 2^32) / hashrate
    double difficulty = GLOBAL_STATE->DEVICE_CONFIG.family.asic.difficulty;
    double results_per_second = (expected_hashrate_ghs * 1e9) / (difficulty * 4294967296.0);

    // Calculate cooldown based on expected results per second
    // cooldown_ms = (1 / results_per_second) * 1000
    double cooldown_seconds = 1.0 / results_per_second;
    uint32_t min_cooldown_ms = (uint32_t)(cooldown_seconds * 1000.0);

    // Ensure reasonable bounds for cooldown
    if (min_cooldown_ms < 500) min_cooldown_ms = 500;     // Minimum 500ms
    if (min_cooldown_ms > 10000) min_cooldown_ms = 10000; // Maximum 10 seconds

    if (current_time - last_result_time < pdMS_TO_TICKS(min_cooldown_ms)) {
        vTaskDelay(1); // Yield to prevent watchdog timeout during cooldown
        return NULL;
    }

    // Convert to probability per ASIC_process_work call
    // ASIC_process_work is called roughly every 100ms, so adjust probability accordingly
    double calls_per_second = 10.0; // ~10 calls per second
    int result_probability = (int)(results_per_second / calls_per_second * 100.0);

    // Ensure minimum probability and reasonable bounds
    if (result_probability < 1) result_probability = 1;     // At least 1% chance
    if (result_probability > 50) result_probability = 50;   // Cap at 50% to prevent overwhelming

    // Log expected performance (only occasionally to avoid spam)
    static uint32_t last_log_time = 0;
    if (current_time - last_log_time > pdMS_TO_TICKS(30000)) { // Log every 30 seconds
        ESP_LOGI(TAG, "ASIC SIMULATION: Expected %.1f GH/s, %.2f results/sec, %d%% probability, %dms cooldown",
                 expected_hashrate_ghs, results_per_second, result_probability, min_cooldown_ms);
        last_log_time = current_time;
    }

    if ((rand() % 100) < result_probability) {
        simulated_result.job_id = simulated_job_id_counter % 128;

        // Generate random nonce - validity will be checked by the mining system
        // This naturally produces ~0.1% invalid nonces due to difficulty requirements
        simulated_result.nonce = rand();

        simulated_result.rolled_version = 0x20000000 | (rand() & 0x1FFF);

        has_pending_result = true;
        last_result_time = current_time;
        ESP_LOGI(TAG, "ASIC SIMULATION: Found nonce 0x%08X for job %d", simulated_result.nonce, simulated_result.job_id);
    }

    if (has_pending_result) {
        has_pending_result = false;
        return &simulated_result;
    }

    vTaskDelay(1); // Yield to prevent watchdog timeout if no result is found
    return NULL;
}

/**
 * @brief Set maximum baud rate
 */
int ASIC_set_max_baud(GlobalState * GLOBAL_STATE) {
    simulated_baud_rate = 1000000; // 1Mbps
    ESP_LOGI(TAG, "ASIC SIMULATION: Set baud rate to %d", simulated_baud_rate);
    return simulated_baud_rate;
}

/**
 * @brief Send work to ASIC
 */
void ASIC_send_work(GlobalState * GLOBAL_STATE, void * next_job) {
    bm_job *job = (bm_job *)next_job;

    // Handle NULL job (queue might be empty)
    if (job == NULL) {
        ESP_LOGD(TAG, "ASIC SIMULATION: Received NULL job, skipping");
        return;
    }

    simulated_job_id_counter = (simulated_job_id_counter + 1) % 128;

    ESP_LOGD(TAG, "ASIC SIMULATION: Sent work with job ID %d", simulated_job_id_counter);

    // Ensure arrays are allocated (defensive programming)
    if (GLOBAL_STATE->ASIC_TASK_MODULE.active_jobs == NULL || GLOBAL_STATE->valid_jobs == NULL) {
        ESP_LOGE(TAG, "ASIC SIMULATION: Arrays not allocated! Skipping job processing.");
        return;
    }

    // Store the job in the global state as if it was sent to hardware
    if (GLOBAL_STATE->ASIC_TASK_MODULE.active_jobs[simulated_job_id_counter] != NULL) {
        free(GLOBAL_STATE->ASIC_TASK_MODULE.active_jobs[simulated_job_id_counter]);
    }
    GLOBAL_STATE->ASIC_TASK_MODULE.active_jobs[simulated_job_id_counter] = job;

    // Mark job as valid - with error handling for uninitialized mutex
    int lock_result = pthread_mutex_lock(&GLOBAL_STATE->valid_jobs_lock);
    if (lock_result == 0) {
        GLOBAL_STATE->valid_jobs[simulated_job_id_counter] = 1;
        pthread_mutex_unlock(&GLOBAL_STATE->valid_jobs_lock);
    } else {
        // Mutex lock failed, possibly because mutex is not initialized in simulation
        // To avoid spamming logs, mark job as valid anyway
        GLOBAL_STATE->valid_jobs[simulated_job_id_counter] = 1;
        ESP_LOGW(TAG, "ASIC SIMULATION: Failed to lock mutex (%d), but marking job as valid", lock_result);
    }
}

/**
 * @brief Set version mask
 */
void ASIC_set_version_mask(GlobalState * GLOBAL_STATE, uint32_t mask) {
    simulated_version_mask = mask;
    ESP_LOGI(TAG, "ASIC SIMULATION: Set version mask to 0x%08X", mask);
}

/**
 * @brief Set ASIC frequency
 */
bool ASIC_set_frequency(GlobalState * GLOBAL_STATE, float target_frequency) {
    simulated_frequency = target_frequency;
    ESP_LOGI(TAG, "ASIC SIMULATION: Set frequency to %.1f MHz", target_frequency);
    return true;
}

/**
 * @brief Get ASIC job frequency in milliseconds
 */
double ASIC_get_asic_job_frequency_ms(GlobalState * GLOBAL_STATE) {
    // Apply small variation to simulated_frequency for realism
    float varied_frequency = simulated_frequency + (simulated_frequency * (((float)rand() / RAND_MAX - 0.5f) * 0.002f)); // +/- 0.1% variation

    // FIXED: Higher frequency should result in LOWER (faster) job times
    // Base timing at 400MHz = 1000ms, scales inversely with frequency
    double base_job_time_ms = 1000.0 * (400.0 / varied_frequency);

    // Adjust for chip count - more chips = faster processing
    double job_freq_ms = base_job_time_ms / simulated_chip_count;

    // Ensure reasonable bounds
    if (job_freq_ms < 100.0) job_freq_ms = 100.0;  // Minimum 100ms
    if (job_freq_ms > 5000.0) job_freq_ms = 5000.0; // Maximum 5 seconds

    ESP_LOGD(TAG, "ASIC SIMULATION: Frequency %.1f MHz, Job time %.2f ms", varied_frequency, job_freq_ms);
    return job_freq_ms;
}

/**
 * @brief Cleanup ASIC simulation resources
 */
void ASIC_sim_cleanup(GlobalState * GLOBAL_STATE) {
    ESP_LOGI(TAG, "ASIC SIMULATION: Cleaning up simulation resources");

    // Note: Arrays and mutex are allocated/deallocated by ASIC_task, not simulation
    // No cleanup needed for simulation-specific resources

    ESP_LOGI(TAG, "ASIC SIMULATION: Cleanup completed");
}

/**
 * @brief Read ASIC registers (simulation)
 */
void ASIC_read_registers(GlobalState * GLOBAL_STATE) {
    ESP_LOGD(TAG, "ASIC SIMULATION: Reading registers (%d chips)", simulated_chip_count);
    
    uint64_t now_us = esp_timer_get_time();
    uint32_t now_ms = (uint32_t)(now_us / 1000ULL);
    
    if (last_register_read_time_ms == 0) {
        last_register_read_time_ms = now_ms;
        return;
    }
    
    uint32_t delta_ms = now_ms - last_register_read_time_ms;
    if (delta_ms == 0) {
        return;
    }
    
    // Calculate expected total hashrate GH/s (reuse real calculation, consistent with process_work)
    double expected_hashrate_ghs = simulated_frequency *
                                   GLOBAL_STATE->DEVICE_CONFIG.family.asic.small_core_count *
                                   GLOBAL_STATE->DEVICE_CONFIG.family.asic_count / 1000.0;
    
    double hashes_per_ms = (expected_hashrate_ghs * 1e9) / 1000.0;  // total hashes per ms
    
    for (uint8_t chip = 0; chip < simulated_chip_count; chip++) {
        double chip_hashes_per_ms = hashes_per_ms / simulated_chip_count;
        uint64_t delta_hashes = (uint64_t)(chip_hashes_per_ms * delta_ms);
        
        // Update counters (simulate uint32 wraparound)
        simulated_total_counters[chip] += (uint32_t)(delta_hashes & 0xFFFFFFFFULL);
        simulated_domain_counters[chip] += (uint32_t)(delta_hashes & 0xFFFFFFFFULL);
        
        uint64_t error_delta = delta_hashes / 1000;  // ~0.1% error rate
        simulated_error_counters[chip] += (uint32_t)error_delta;
        
        // Total count register
        hashrate_monitor_register_read(GLOBAL_STATE, REGISTER_TOTAL_COUNT, chip, simulated_total_counters[chip]);
        
        // Domain 0 count register (matching real update_hashrate behavior)
        hashrate_monitor_register_read(GLOBAL_STATE, REGISTER_DOMAIN_0_COUNT, chip, simulated_domain_counters[chip]);
        
        // Error count register
        hashrate_monitor_register_read(GLOBAL_STATE, REGISTER_ERROR_COUNT, chip, simulated_error_counters[chip]);
        
        // Hashrate register (short-term, matching real format: no bit31/flag_long, value != 0x007FFFFF)
        double chip_ghs = expected_hashrate_ghs / GLOBAL_STATE->DEVICE_CONFIG.family.asic_count;
        // Realistic variation +/- 2%
        float variation = 1.0f + (((float)esp_random() / (float)0xFFFFFFFFU) * 0.04f - 0.02f);
        chip_ghs *= variation;
        
        // Precise floating-point calculation, floor to uint32_t, mask to 31 bits (no flag_long)
        double hr_double = chip_ghs * 1e9 / SIM_HASH_RATE_UNIT;
        uint32_t hr_value = (uint32_t)hr_double & 0x7FFFFFFFU;
        
        // Clamp to avoid invalid value 0x007FFFFF
        if (hr_value == 0x007FFFFF) hr_value = 0x007FFFFE;
        
        hashrate_monitor_register_read(GLOBAL_STATE, REGISTER_HASHRATE, chip, hr_value);
    }
    
    last_register_read_time_ms = now_ms;
    
    ESP_LOGD(TAG, "ASIC SIMULATION: Updated %d chips, delta=%u ms, expected=%.1f GH/s total",
             simulated_chip_count, delta_ms, expected_hashrate_ghs);
}