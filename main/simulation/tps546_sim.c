#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <esp_log.h>
#include <esp_random.h>
#include <string.h>
#include "TPS546.h"

static const char *TAG = "TPS546_SIM";

// Simulated state
static TPS546_CONFIG simulated_config;
static float simulated_vin = 12.0f;    // Input voltage
static float simulated_vout = 1.2f;    // Output voltage
static float simulated_iout = 2.5f;    // Output current
static int simulated_temperature = 45; // Temperature in Celsius
static int simulated_frequency = 650;  // Switching frequency in kHz
static uint16_t simulated_status_word = 0; // Status word
static char simulated_error_message[256] = "No faults detected";

/**
 * @brief Initialize the TPS546 regulator simulation
 */
esp_err_t TPS546_init(TPS546_CONFIG config) {
    ESP_LOGI(TAG, "TPS546 SIMULATION: Initializing simulated voltage regulator");

    // Seed random number generator
    srand(esp_random());

    // Store configuration
    simulated_config = config;

    // Initialize simulated values based on config with a small variation (0.1%)
    float vout_base = config.TPS546_INIT_VOUT_COMMAND;
    simulated_vout = vout_base + (vout_base * (((float)rand() / RAND_MAX - 0.5f) * 0.002f)); // +/- 0.1% variation
    // Use config VIN_ON as base for input voltage
    simulated_vin = config.TPS546_INIT_VIN_ON + ((float)rand() / RAND_MAX - 0.5f) * 0.4f; // +/- 0.2V variation around VIN_ON
    simulated_iout = 2.0f + ((float)rand() / RAND_MAX) * 2.0f; // 2-4A
    simulated_temperature = 40 + (rand() % 20); // 40-60°C
    simulated_frequency = TPS546_INIT_FREQUENCY;
    simulated_status_word = 0; // No faults initially

    ESP_LOGI(TAG, "TPS546 SIMULATION: Initialized with:");
    ESP_LOGI(TAG, "  Vout: %.2fV, Vin: %.2fV, Iout: %.2fA", simulated_vout, simulated_vin, simulated_iout);
    ESP_LOGI(TAG, "  Temperature: %d°C, Frequency: %d kHz", simulated_temperature, simulated_frequency);

    return ESP_OK;
}

esp_err_t TPS546_clear_faults(void) {
    simulated_status_word = 0; // Clear all faults
    strcpy(simulated_error_message, "No faults detected");
    ESP_LOGI(TAG, "TPS546 SIMULATION: Cleared all faults");
    return ESP_OK;
}

void TPS546_read_mfr_info(uint8_t *read_mfr_revision) {
    // Simulate manufacturer info
    read_mfr_revision[0] = 0x01;
    read_mfr_revision[1] = 0x02;
    read_mfr_revision[2] = 0x03;
    ESP_LOGI(TAG, "TPS546 SIMULATION: Read manufacturer info");
}

void TPS546_write_entire_config(void) {
    ESP_LOGI(TAG, "TPS546 SIMULATION: Writing configuration (simulated)");
    // In simulation, just log that we're writing config
}

int TPS546_get_frequency(void) {
    ESP_LOGD(TAG, "TPS546 SIMULATION: Returning frequency %d kHz", simulated_frequency);
    return simulated_frequency;
}

void TPS546_set_frequency(int newfreq) {
    simulated_frequency = newfreq;
    ESP_LOGI(TAG, "TPS546 SIMULATION: Set frequency to %d kHz", newfreq);
}

int TPS546_get_temperature(void) {
    // Add small random variation
    int variation = (rand() % 6) - 3; // -3 to +3°C variation
    int temp = simulated_temperature + variation;

    // Keep temperature in reasonable range
    if (temp < 30) temp = 30;
    if (temp > 80) temp = 80;

    // Slowly drift temperature
    simulated_temperature += (rand() % 3) - 1; // -1 to +1°C drift

    ESP_LOGD(TAG, "TPS546 SIMULATION: Returning temperature %d°C", temp);
    return temp;
}

float TPS546_get_vin(void) {
    // Add small random variation
    float variation = ((float)rand() / RAND_MAX - 0.5f) * 0.2f; // -0.1 to +0.1V variation
    float vin = simulated_vin + variation;

    // Keep voltage in reasonable range
    if (vin < simulated_config.TPS546_INIT_VIN_OFF) vin = simulated_config.TPS546_INIT_VIN_OFF;
    if (vin > simulated_config.TPS546_INIT_VIN_OV_FAULT_LIMIT) vin = simulated_config.TPS546_INIT_VIN_OV_FAULT_LIMIT;

    // Slowly drift voltage
    simulated_vin += ((float)rand() / RAND_MAX - 0.5f) * 0.05f;

    ESP_LOGD(TAG, "TPS546 SIMULATION: Returning Vin %.2fV", vin);
    return vin;
}

float TPS546_get_iout(void) {
    // Add small random variation
    float variation = ((float)rand() / RAND_MAX - 0.5f) * 0.3f; // -0.15 to +0.15A variation
    float iout = simulated_iout + variation;

    // Keep current in reasonable range
    if (iout < 0.5f) iout = 0.5f;
    if (iout > 6.0f) iout = 6.0f;

    // Slowly drift current
    simulated_iout += ((float)rand() / RAND_MAX - 0.5f) * 0.1f;

    ESP_LOGD(TAG, "TPS546 SIMULATION: Returning Iout %.2fA", iout);
    return iout;
}

float TPS546_get_vout(void) {
    // Add very small random variation
    float variation = ((float)rand() / RAND_MAX - 0.5f) * (simulated_vout * 0.002f); // +/- 0.1% variation
    float vout = simulated_vout + variation;

    // Keep voltage very close to target
    if (vout < simulated_config.TPS546_INIT_VOUT_MIN) vout = simulated_config.TPS546_INIT_VOUT_MIN;
    if (vout > simulated_config.TPS546_INIT_VOUT_MAX) vout = simulated_config.TPS546_INIT_VOUT_MAX;

    ESP_LOGD(TAG, "TPS546 SIMULATION: Returning Vout %.3fV", vout);
    return vout;
}

esp_err_t TPS546_set_vout(float volts) {
    if (volts < simulated_config.TPS546_INIT_VOUT_MIN || volts > simulated_config.TPS546_INIT_VOUT_MAX) {
        ESP_LOGE(TAG, "TPS546 SIMULATION: Voltage %.2fV out of range [%.2f, %.2f]",
                 volts, simulated_config.TPS546_INIT_VOUT_MIN, simulated_config.TPS546_INIT_VOUT_MAX);
        return ESP_FAIL;
    }

    simulated_vout = volts;
    ESP_LOGI(TAG, "TPS546 SIMULATION: Set Vout to %.2fV", volts);
    return ESP_OK;
}

void TPS546_show_voltage_settings(void) {
    ESP_LOGI(TAG, "TPS546 SIMULATION: Voltage Settings:");
    ESP_LOGI(TAG, "  VIN_ON: %.2fV", simulated_config.TPS546_INIT_VIN_ON);
    ESP_LOGI(TAG, "  VIN_OFF: %.2fV", simulated_config.TPS546_INIT_VIN_OFF);
    ESP_LOGI(TAG, "  VOUT_COMMAND: %.2fV", simulated_vout);
    ESP_LOGI(TAG, "  VOUT_MIN: %.2fV", simulated_config.TPS546_INIT_VOUT_MIN);
    ESP_LOGI(TAG, "  VOUT_MAX: %.2fV", simulated_config.TPS546_INIT_VOUT_MAX);
}

esp_err_t TPS546_check_status(GlobalState * GLOBAL_STATE) {
    // Simulate occasional status changes
    if ((rand() % 1000) < 1) { // 0.1% chance of status change (extremely rare)
        simulated_status_word = rand() % 0xFFFF; // Random status
        if (simulated_status_word & TPS546_STATUS_TEMP) {
            strcpy(simulated_error_message, "Simulated temperature fault");
        } else if (simulated_status_word & TPS546_STATUS_IOUT_OC) {
            strcpy(simulated_error_message, "Simulated overcurrent fault");
        } else {
            strcpy(simulated_error_message, "Simulated fault detected");
        }
    }

    // Update global state if there are faults
    if (simulated_status_word != 0) {
        GLOBAL_STATE->SYSTEM_MODULE.power_fault = 1;
        ESP_LOGW(TAG, "TPS546 SIMULATION: Status word: 0x%04X - %s", simulated_status_word, simulated_error_message);
    } else {
        GLOBAL_STATE->SYSTEM_MODULE.power_fault = 0;
    }

    return ESP_OK;
}

const char* TPS546_get_error_message(void) {
    return simulated_error_message;
}