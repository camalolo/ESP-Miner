#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <esp_log.h>
#include <esp_random.h>
#include "EMC2103.h"

static const char *TAG = "EMC2103_SIM";

// Simulated state
static float simulated_external_temp1 = 40.0f;
static float simulated_external_temp2 = 38.0f;
static uint16_t simulated_fan_speed = 1500; // RPM
static float simulated_fan_speed_percent = 0.5f; // 50%
static uint8_t simulated_ideality_factor = 0x12; // Default
static uint8_t simulated_beta_compensation = 0x08; // Default

/**
 * @brief Initialize the EMC2103 sensor simulation.
 *
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t EMC2103_init() {
    ESP_LOGI(TAG, "EMC2103 SIMULATION: Initializing simulated temperature sensor");

    // Seed random number generator
    srand(esp_random());

    // Initialize simulated values
    simulated_external_temp1 = 40.0f + ((float)rand() / RAND_MAX) * 20.0f; // 40-60°C
    simulated_external_temp2 = 40.0f + ((float)rand() / RAND_MAX) * 20.0f; // 40-60°C
    simulated_fan_speed = 1200 + (rand() % 800); // 1200-2000 RPM
    simulated_fan_speed_percent = 0.5f;

    ESP_LOGI(TAG, "EMC2103 SIMULATION: Initialized with external_temp1=%.1f°C, external_temp2=%.1f°C, fan_speed=%d RPM",
             simulated_external_temp1, simulated_external_temp2, simulated_fan_speed);

    return ESP_OK;
}

esp_err_t EMC2103_set_ideality_factor(uint8_t ideality) {
    simulated_ideality_factor = ideality;
    ESP_LOGI(TAG, "EMC2103 SIMULATION: Set ideality factor to 0x%02X for both diodes", ideality);
    return ESP_OK;
}

esp_err_t EMC2103_set_beta_compensation(uint8_t beta) {
    simulated_beta_compensation = beta;
    ESP_LOGI(TAG, "EMC2103 SIMULATION: Set beta compensation to 0x%02X for both diodes", beta);
    return ESP_OK;
}

/**
 * @brief Set the fan speed as a percentage.
 *
 * @param percent The desired fan speed as a percentage (0.0 to 1.0).
 */
esp_err_t EMC2103_set_fan_speed(float percent) {
    if (percent < 0.0f) percent = 0.0f;
    if (percent > 1.0f) percent = 1.0f;

    simulated_fan_speed_percent = percent;
    // Simulate fan speed based on percentage (roughly linear relationship)
    simulated_fan_speed = (uint16_t)(600 + percent * 2400); // 600-3000 RPM range

    ESP_LOGI(TAG, "EMC2103 SIMULATION: Set fan speed to %.1f%% (%d RPM)", percent * 100.0f, simulated_fan_speed);
    return ESP_OK;
}

/**
 * @brief Get the current fan speed in RPM.
 *
 * @return uint16_t The fan speed in RPM.
 */
uint16_t EMC2103_get_fan_speed(void) {
    // Add some random variation to simulate real-world fluctuations
    int variation = (rand() % 200) - 100; // -100 to +100 RPM variation
    uint16_t current_speed = simulated_fan_speed + variation;

    if (current_speed < 300) current_speed = 300; // Minimum speed
    if (current_speed > 3500) current_speed = 3500; // Maximum speed

    ESP_LOGD(TAG, "EMC2103 SIMULATION: Returning fan speed %d RPM", current_speed);
    return current_speed;
}

/**
 * @brief Get the external temperature in Celsius.
 *
 * @return float The external temperature in Celsius.
 */
float EMC2103_get_external_temp(void) {
    // Add small random variation to simulate temperature fluctuations
    float variation = ((float)rand() / RAND_MAX - 0.5f) * 3.0f; // -1.5 to +1.5°C variation
    float current_temp = simulated_external_temp1 + variation;

    // Keep temperature in reasonable range
    if (current_temp < 25.0f) current_temp = 25.0f;
    if (current_temp > 70.0f) current_temp = 70.0f;

    // Slowly drift temperature over time to simulate real behavior
    simulated_external_temp1 += ((float)rand() / RAND_MAX - 0.5f) * 0.2f; // Small drift

    ESP_LOGD(TAG, "EMC2103 SIMULATION: Returning external temperature 1 %.2f°C", current_temp);
    return current_temp;
}

/**
 * @brief Get the external temperature 2 in Celsius.
 *
 * @return float The external temperature 2 in Celsius.
 */
float EMC2103_get_external_temp2(void) {
    // Add small random variation
    float variation = ((float)rand() / RAND_MAX - 0.5f) * 3.0f; // -1.5 to +1.5°C variation
    float current_temp = simulated_external_temp2 + variation;

    // Keep temperature in reasonable range
    if (current_temp < 25.0f) current_temp = 25.0f;
    if (current_temp > 70.0f) current_temp = 70.0f;

    // Slowly drift temperature over time
    simulated_external_temp2 += ((float)rand() / RAND_MAX - 0.5f) * 0.15f; // Smaller drift

    ESP_LOGD(TAG, "EMC2103 SIMULATION: Returning external temperature 2 %.2f°C", current_temp);
    return current_temp;
}