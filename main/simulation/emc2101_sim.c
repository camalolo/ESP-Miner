#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <esp_log.h>
#include <esp_random.h>
#include "EMC2101.h"

static const char *TAG = "EMC2101_SIM";

// Simulated state
static float simulated_external_temp = 35.0f;
static float simulated_internal_temp = 32.0f;
static uint16_t simulated_fan_speed = 1200; // RPM
static float simulated_fan_speed_percent = 0.5f; // 50%
static uint8_t simulated_ideality_factor = EMC2101_IDEALITY_1_0080;
static uint8_t simulated_beta_compensation = EMC2101_BETA_AUTO;

/**
 * @brief Initialize the EMC2101 sensor simulation.
 *
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t EMC2101_init() {
    ESP_LOGI(TAG, "EMC2101 SIMULATION: Initializing simulated temperature sensor");

    // Seed random number generator
    srand(esp_random());

    // Initialize simulated values
    simulated_external_temp = 40.0f + ((float)rand() / RAND_MAX) * 20.0f; // 40-60°C
    simulated_internal_temp = 35.0f + ((float)rand() / RAND_MAX) * 15.0f;  // 35-50°C
    simulated_fan_speed = 1000 + (rand() % 1000); // 1000-2000 RPM
    simulated_fan_speed_percent = 0.5f;

    ESP_LOGI(TAG, "EMC2101 SIMULATION: Initialized with external_temp=%.1f°C, internal_temp=%.1f°C, fan_speed=%d RPM",
             simulated_external_temp, simulated_internal_temp, simulated_fan_speed);

    return ESP_OK;
}

esp_err_t EMC2101_set_ideality_factor(uint8_t ideality) {
    simulated_ideality_factor = ideality;
    ESP_LOGI(TAG, "EMC2101 SIMULATION: Set ideality factor to 0x%02X", ideality);
    return ESP_OK;
}

esp_err_t EMC2101_set_beta_compensation(uint8_t beta) {
    simulated_beta_compensation = beta;
    ESP_LOGI(TAG, "EMC2101 SIMULATION: Set beta compensation to 0x%02X", beta);
    return ESP_OK;
}

// takes a fan speed percent
esp_err_t EMC2101_set_fan_speed(float percent) {
    if (percent < 0.0f) percent = 0.0f;
    if (percent > 1.0f) percent = 1.0f;

    simulated_fan_speed_percent = percent;
    // Simulate fan speed based on percentage (roughly linear relationship)
    simulated_fan_speed = (uint16_t)(500 + percent * 2000); // 500-2500 RPM range

    ESP_LOGI(TAG, "EMC2101 SIMULATION: Set fan speed to %.1f%% (%d RPM)", percent * 100.0f, simulated_fan_speed);
    return ESP_OK;
}

// RPM = 5400000/reading
uint16_t EMC2101_get_fan_speed(void) {
    // Add some random variation to simulate real-world fluctuations
    int variation = (rand() % 100) - 50; // -50 to +50 RPM variation
    uint16_t current_speed = simulated_fan_speed + variation;

    if (current_speed < 500) current_speed = 500; // Minimum speed
    if (current_speed > 3000) current_speed = 3000; // Maximum speed

    ESP_LOGD(TAG, "EMC2101 SIMULATION: Returning fan speed %d RPM", current_speed);
    return current_speed;
}

float EMC2101_get_external_temp(void) {
    // Add small random variation to simulate temperature fluctuations
    float variation = ((float)rand() / RAND_MAX - 0.5f) * 2.0f; // -1.0 to +1.0°C variation
    float current_temp = simulated_external_temp + variation;

    // Keep temperature in reasonable range
    if (current_temp < 20.0f) current_temp = 20.0f;
    if (current_temp > 60.0f) current_temp = 60.0f;

    // Slowly drift temperature over time to simulate real behavior
    simulated_external_temp += ((float)rand() / RAND_MAX - 0.5f) * 0.1f; // Small drift

    ESP_LOGD(TAG, "EMC2101 SIMULATION: Returning external temperature %.2f°C", current_temp);
    return current_temp;
}

float EMC2101_get_internal_temp(void) {
    // Add small random variation
    float variation = ((float)rand() / RAND_MAX - 0.5f) * 1.0f; // -0.5 to +0.5°C variation
    float current_temp = simulated_internal_temp + variation;

    // Keep temperature in reasonable range
    if (current_temp < 25.0f) current_temp = 25.0f;
    if (current_temp > 50.0f) current_temp = 50.0f;

    // Slowly drift temperature over time
    simulated_internal_temp += ((float)rand() / RAND_MAX - 0.5f) * 0.05f; // Smaller drift

    ESP_LOGD(TAG, "EMC2101 SIMULATION: Returning internal temperature %.2f°C", current_temp);
    return current_temp;
}