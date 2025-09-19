#include "nvs_config.h"
#include "esp_log.h"

static const char *TAG = "device_config_sim";

// Initialize NVS with simulation defaults if not present
void device_config_sim_init(void) {
    ESP_LOGI(TAG, "Initializing NVS with simulation defaults");

    // Check and set device model
    char *device_model = nvs_config_get_string(NVS_CONFIG_DEVICE_MODEL, "__NOT_SET__");
    if (strcmp(device_model, "__NOT_SET__") == 0) {
        nvs_config_set_string(NVS_CONFIG_DEVICE_MODEL, "Supra");  // Set to Supra for 401
        ESP_LOGI(TAG, "Set default device model: Supra");
    }
    free(device_model);

    // Check and set ASIC model
    char *asic_model = nvs_config_get_string(NVS_CONFIG_ASIC_MODEL, "__NOT_SET__");
    if (strcmp(asic_model, "__NOT_SET__") == 0) {
        nvs_config_set_string(NVS_CONFIG_ASIC_MODEL, "BM1368");  // BM1368 for Supra
        ESP_LOGI(TAG, "Set default ASIC model: BM1368");
    }
    free(asic_model);

    // Check and set board version
    char *board_version = nvs_config_get_string(NVS_CONFIG_BOARD_VERSION, "__NOT_SET__");
    if (strcmp(board_version, "__NOT_SET__") == 0) {
        nvs_config_set_string(NVS_CONFIG_BOARD_VERSION, "402");  // Supra board version with TPS546
        ESP_LOGI(TAG, "Set default board version: 402");
    }
    free(board_version);

    // Enable simulation components consistent with 401 Supra
    if (nvs_config_get_u16(NVS_CONFIG_EMC2101, 0) == 0) {
        nvs_config_set_u16(NVS_CONFIG_EMC2101, 1);
        ESP_LOGI(TAG, "Enabled EMC2101 for simulation");
    }
    if (nvs_config_get_u16(NVS_CONFIG_EMC2103, 0) == 0) {
        nvs_config_set_u16(NVS_CONFIG_EMC2103, 0);  // Disable EMC2103 for Supra
        ESP_LOGI(TAG, "Disabled EMC2103 for simulation");
    }
    if (nvs_config_get_u16(NVS_CONFIG_INA260, 0) == 0) {
        nvs_config_set_u16(NVS_CONFIG_INA260, 1);
        ESP_LOGI(TAG, "Enabled INA260 for simulation");
    }
    if (nvs_config_get_u16(NVS_CONFIG_TPS546, 0) == 0) {
        nvs_config_set_u16(NVS_CONFIG_TPS546, 1);  // Enable TPS546 for simulation
        ESP_LOGI(TAG, "Enabled TPS546 for simulation");
    }
    if (nvs_config_get_u16(NVS_CONFIG_DS4432U, 0) == 0) {
        nvs_config_set_u16(NVS_CONFIG_DS4432U, 1);
        ESP_LOGI(TAG, "Enabled DS4432U for simulation");
    }

    // Disable display in dev board mode
    nvs_config_set_string(NVS_CONFIG_DISPLAY, "NONE");
    ESP_LOGI(TAG, "Disabled display for dev board simulation");

    ESP_LOGI(TAG, "NVS simulation defaults initialized");
}