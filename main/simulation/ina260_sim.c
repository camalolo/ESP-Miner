#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <esp_log.h>
#include <esp_random.h>
#include "INA260.h"

static const char *TAG = "INA260_SIM";

// Simulated state (voltage in mV, current in mA, power in W)
static float simulated_voltage = 12000.0f; // 12V typical (in mV)
static float simulated_current = 2500.0f;  // 2.5A typical (in mA)
static float simulated_power = 30000.0f;   // 30W typical (in mW)

/**
 * @brief Initialize the INA260 sensor simulation.
 *
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t INA260_init(void) {
    ESP_LOGI(TAG, "INA260 SIMULATION: Initializing simulated power monitor");

    // Seed random number generator
    srand(esp_random());

    // Initialize simulated values (in mV, mA, mW)
    simulated_voltage = 4800.0f + ((float)rand() / RAND_MAX) * 400.0f; // 4800-5200mV
    simulated_current = 2500.0f + ((float)rand() / RAND_MAX) * 500.0f;  // 2500-3000mA (aiming for ~2.8A)
    simulated_power = (simulated_voltage / 1000.0f) * (simulated_current / 1000.0f) * 1000.0f; // Power in mW

    ESP_LOGI(TAG, "INA260 SIMULATION: Initialized with voltage=%.2fmV, current=%.2fmA, power=%.2fmW",
             simulated_voltage, simulated_current, simulated_power);
    ESP_LOGD(TAG, "INA260 SIMULATION: Initialized simulated_voltage=%.2fmV, simulated_current=%.2fmA, simulated_power=%.2fW",
             simulated_voltage, simulated_current, simulated_power);

    return ESP_OK;
}

bool INA260_installed(void) {
    ESP_LOGD(TAG, "INA260 SIMULATION: Device reported as installed");
    return true; // Always report as installed in simulation
}

float INA260_read_current(void) {
    // Add small random variation to simulate real-world fluctuations
    float variation = ((float)rand() / RAND_MAX - 0.5f) * 200.0f; // -100 to +100mA variation
    float current = simulated_current + variation;

    // Keep current in reasonable range (500-5000mA)
    if (current < 500.0f) current = 500.0f;
    if (current > 5000.0f) current = 5000.0f;

    // Slowly drift current over time to simulate load changes
    simulated_current += ((float)rand() / RAND_MAX - 0.5f) * 50.0f; // Small drift (in mA)

    ESP_LOGD(TAG, "INA260 SIMULATION: Returning current %.3fmA (simulated_current: %.3fmA)", current, simulated_current);
    return current;
}

float INA260_read_voltage(void) {
    // Add small random variation
    float variation = ((float)rand() / RAND_MAX - 0.5f) * 100.0f; // -50 to +50mV variation
    float voltage = simulated_voltage + variation;

    // Keep voltage in reasonable range (4800-5200mV)
    if (voltage < 4800.0f) voltage = 4800.0f;
    if (voltage > 5200.0f) voltage = 5200.0f;

    // Slowly drift voltage over time (in mV)
    simulated_voltage += ((float)rand() / RAND_MAX - 0.5f) * 20.0f; // Smaller drift

    ESP_LOGD(TAG, "INA260 SIMULATION: Returning voltage %.3fmV (simulated_voltage: %.3fmV)", voltage, simulated_voltage);
    return voltage;
}

float INA260_read_power(void) {
    // Calculate power from current simulated values
    float current = INA260_read_current();
    float voltage = INA260_read_voltage();
    // Calculate power from current simulated values (in mW)
    // Convert mV and mA to V and A for calculation, then back to mW
    float power = (current / 1000.0f) * (voltage / 1000.0f) * 1000.0f;

    // Update simulated power
    simulated_power = power;

    ESP_LOGD(TAG, "INA260 SIMULATION: Calculated power: %.3fmW (V:%.3fmV, I:%.3fmA)", power, voltage, current);
    ESP_LOGD(TAG, "INA260 SIMULATION: Returning power %.3fmW", power);
    return power;
}