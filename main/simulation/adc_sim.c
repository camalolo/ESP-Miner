#include "esp_log.h"
#include "adc.h"
#include "nvs_config.h"
#include "esp_random.h"

static const char *TAG = "ADC_SIM";

void ADC_init(void) {
    ESP_LOGI(TAG, "ADC SIMULATION: Initializing simulated ADC");
    // No hardware initialization needed for simulation
}

uint16_t ADC_get_vcore(void) {
    // Read the configured ASIC voltage from NVS and simulate close to that value
    uint16_t configured_voltage_mv = nvs_config_get_u16(NVS_CONFIG_ASIC_VOLTAGE, 1166);
    // Add small random variation +/- 10mV for realism
    int16_t variation = (esp_random() % 21) - 10; // -10 to +10
    uint16_t simulated_voltage = (configured_voltage_mv + variation > 0) ? (configured_voltage_mv + variation) : configured_voltage_mv;

    ESP_LOGD(TAG, "ADC SIMULATION: Returning simulated vcore voltage: %d mV (configured: %d mV)", simulated_voltage, configured_voltage_mv);
    return simulated_voltage;
}