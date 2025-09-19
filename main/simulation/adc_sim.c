#include "esp_log.h"
#include "adc.h"
#include "nvs_config.h"
#include "esp_random.h"

static const char *TAG = "ADC_SIM";

static uint16_t cached_configured_voltage_mv = 0;
static bool voltage_cached = false;

void ADC_init(void) {
    ESP_LOGI(TAG, "ADC SIMULATION: Initializing simulated ADC");
    // NVS not yet initialized at this point; cache will be populated on first read
}

uint16_t ADC_get_vcore(void) {
    // Lazily cache the configured voltage (NVS may not be ready on very first calls)
    if (!voltage_cached) {
        cached_configured_voltage_mv = nvs_config_get_u16(NVS_CONFIG_ASIC_VOLTAGE);
        voltage_cached = true;
    }

    // Add small random variation +/- 10mV for realism
    int16_t variation = (esp_random() % 21) - 10; // -10 to +10
    uint16_t simulated_voltage = (cached_configured_voltage_mv + variation > 0) ? (cached_configured_voltage_mv + variation) : cached_configured_voltage_mv;

    ESP_LOGD(TAG, "ADC SIMULATION: Returning simulated vcore voltage: %d mV (configured: %d mV)", simulated_voltage, cached_configured_voltage_mv);
    return simulated_voltage;
}