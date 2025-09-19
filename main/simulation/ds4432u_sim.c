#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <esp_log.h>
#include <esp_random.h>
#include <math.h>
#include "DS4432U.h"

static const char *TAG = "DS4432U_SIM";

// Simulated state
static uint8_t simulated_out0_code = 0x00;
static uint8_t simulated_out1_code = 0x00;

// DS4432U Transfer function constants for Bitaxe board (same as real driver)
#define BITAXE_IFS 0.000098921
#define BITAXE_RA 4750.0
#define BITAXE_RB 3320.0
#define BITAXE_VNOM 1.451
#define BITAXE_VMAX 2.39
#define BITAXE_VMIN 0.046
#define TPS40305_VFB 0.6

/**
 * @brief Initialize the DS4432U+ sensor simulation.
 *
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t DS4432U_init(void) {
    ESP_LOGI(TAG, "DS4432U SIMULATION: Initializing simulated current DAC");

    // Seed random number generator
    srand(esp_random());

    // Initialize simulated values
    simulated_out0_code = 0x00;
    simulated_out1_code = 0x00;

    ESP_LOGI(TAG, "DS4432U SIMULATION: Initialized with OUT0=0x%02X, OUT1=0x%02X",
             simulated_out0_code, simulated_out1_code);

    return ESP_OK;
}

/**
 * @brief Set the current DAC code for a specific DS4432U output.
 *
 * @param output The output channel (0 or 1).
 * @param code The current code value to set.
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t DS4432U_set_current_code(uint8_t output, uint8_t code) {
    if (output == 0) {
        simulated_out0_code = code;
        ESP_LOGI(TAG, "DS4432U SIMULATION: Set OUT0 code to 0x%02X", code);
    } else if (output == 1) {
        simulated_out1_code = code;
        ESP_LOGI(TAG, "DS4432U SIMULATION: Set OUT1 code to 0x%02X", code);
    } else {
        ESP_LOGE(TAG, "DS4432U SIMULATION: Invalid output channel %d", output);
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t DS4432U_set_voltage(float vout) {
    float change;
    uint8_t reg;

    // make sure the requested voltage is within range of BITAXE_VMIN and BITAXE_VMAX
    if (vout > BITAXE_VMAX || vout < BITAXE_VMIN) {
        ESP_LOGE(TAG, "DS4432U SIMULATION: Voltage %.3fV out of range [%.3f, %.3f]",
                 vout, BITAXE_VMIN, BITAXE_VMAX);
        return ESP_FAIL;
    }

    // this is the transfer function. comes from the DS4432U+ datasheet
    change = fabs((((TPS40305_VFB / BITAXE_RB) - ((vout - TPS40305_VFB) / BITAXE_RA)) / BITAXE_IFS) * 127);
    reg = (uint8_t)ceil(change);

    // Set the MSB high if the requested voltage is BELOW nominal
    if (vout < BITAXE_VNOM) {
        reg |= 0x80;
    }

    ESP_LOGI(TAG, "DS4432U SIMULATION: Setting voltage %.3fV -> code 0x%02X", vout, reg);
    return DS4432U_set_current_code(0, reg);
}

/**
 * @brief Get the current DAC code value for a specific DS4432U output.
 *
 * @param output The output channel (0 or 1).
 * @param code Pointer to store the current code value.
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t DS4432U_get_current_code(uint8_t output, uint8_t *code) {
    if (output == 0) {
        *code = simulated_out0_code;
        ESP_LOGD(TAG, "DS4432U SIMULATION: Read OUT0 code 0x%02X", *code);
    } else if (output == 1) {
        *code = simulated_out1_code;
        ESP_LOGD(TAG, "DS4432U SIMULATION: Read OUT1 code 0x%02X", *code);
    } else {
        ESP_LOGE(TAG, "DS4432U SIMULATION: Invalid output channel %d", output);
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t DS4432U_test(void) {
    uint8_t data;

    /* Read the DS4432U+ OUT0 register */
    ESP_RETURN_ON_ERROR(DS4432U_get_current_code(0, &data), TAG, "Failed to read simulated DS4432U+ OUT0 register");
    ESP_LOGI(TAG, "DS4432U SIMULATION: OUT0 = 0x%02X", data);

    return ESP_OK;
}