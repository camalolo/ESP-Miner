#include <stdio.h>
#include <esp_log.h>
#include "emc2101_sim.h"
#include "ina260_sim.h"
#include "tps546_sim.h"
#include "ds4432u_sim.h"
#include "emc2103_sim.h"
#include "asic_sim.h"
#include "device_config_sim.h"
#include "../../main/adc.h"
#include "../../main/power/asic_reset.h"
#include "../../components/asic/include/serial.h"

static const char *TAG = "SIMULATION_TEST";

/**
 * @brief Test EMC2101 simulation
 */
void test_emc2101_simulation() {
    ESP_LOGI(TAG, "Testing EMC2101 simulation...");

    esp_err_t ret = EMC2101_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "EMC2101 simulation init failed");
        return;
    }

    // Test temperature readings
    float ext_temp = EMC2101_get_external_temp();
    float int_temp = EMC2101_get_internal_temp();

    ESP_LOGI(TAG, "EMC2101 Test - External temp: %.2f°C, Internal temp: %.2f°C", ext_temp, int_temp);

    // Test fan control
    EMC2101_set_fan_speed(0.5f);
    uint16_t fan_speed = EMC2101_get_fan_speed();
    ESP_LOGI(TAG, "EMC2101 Test - Fan speed: %d RPM", fan_speed);

    ESP_LOGI(TAG, "EMC2101 simulation test completed");
}

/**
 * @brief Test INA260 simulation
 */
void test_ina260_simulation() {
    ESP_LOGI(TAG, "Testing INA260 simulation...");

    esp_err_t ret = INA260_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "INA260 simulation init failed");
        return;
    }

    // Test measurements
    float voltage = INA260_read_voltage();
    float current = INA260_read_current();
    float power = INA260_read_power();

    ESP_LOGI(TAG, "INA260 Test - Voltage: %.3fV, Current: %.3fA, Power: %.3fW",
             voltage, current, power);

    ESP_LOGI(TAG, "INA260 simulation test completed");
}

/**
 * @brief Test TPS546 simulation
 */
void test_tps546_simulation() {
    ESP_LOGI(TAG, "Testing TPS546 simulation...");

    TPS546_CONFIG config = {
        .TPS546_INIT_VOUT_COMMAND = 1.2f,
        .TPS546_INIT_VOUT_MIN = 1.0f,
        .TPS546_INIT_VOUT_MAX = 2.0f
    };

    esp_err_t ret = TPS546_init(config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "TPS546 simulation init failed");
        return;
    }

    // Test measurements
    float vin = TPS546_get_vin();
    float vout = TPS546_get_vout();
    float iout = TPS546_get_iout();
    int temp = TPS546_get_temperature();

    ESP_LOGI(TAG, "TPS546 Test - Vin: %.2fV, Vout: %.3fV, Iout: %.2fA, Temp: %d°C",
             vin, vout, iout, temp);

    // Test voltage setting
    TPS546_set_vout(1.5f);
    vout = TPS546_get_vout();
    ESP_LOGI(TAG, "TPS546 Test - Set Vout to 1.5V, reading: %.3fV", vout);

    ESP_LOGI(TAG, "TPS546 simulation test completed");
}

/**
 * @brief Test DS4432U simulation
 */
void test_ds4432u_simulation() {
    ESP_LOGI(TAG, "Testing DS4432U simulation...");

    esp_err_t ret = DS4432U_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "DS4432U simulation init failed");
        return;
    }

    // Test voltage setting
    DS4432U_set_voltage(1.8f);

    // Test current code setting
    DS4432U_set_current_code(0, 0x80);
    DS4432U_set_current_code(1, 0x40);

    uint8_t code0, code1;
    DS4432U_get_current_code(0, &code0);
    DS4432U_get_current_code(1, &code1);

    ESP_LOGI(TAG, "DS4432U Test - OUT0 code: 0x%02X, OUT1 code: 0x%02X", code0, code1);

    ESP_LOGI(TAG, "DS4432U simulation test completed");
}

/**
 * @brief Test EMC2103 simulation
 */
void test_emc2103_simulation() {
    ESP_LOGI(TAG, "Testing EMC2103 simulation...");

    esp_err_t ret = EMC2103_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "EMC2103 simulation init failed");
        return;
    }

    // Test temperature readings
    float ext_temp1 = EMC2103_get_external_temp();
    float ext_temp2 = EMC2103_get_external_temp2();

    ESP_LOGI(TAG, "EMC2103 Test - External temp1: %.2f°C, External temp2: %.2f°C",
             ext_temp1, ext_temp2);

    // Test fan control
    EMC2103_set_fan_speed(0.7f);
    uint16_t fan_speed = EMC2103_get_fan_speed();
    ESP_LOGI(TAG, "EMC2103 Test - Fan speed: %d RPM", fan_speed);

    ESP_LOGI(TAG, "EMC2103 simulation test completed");
}

/**
 * @brief Test ASIC simulation
 */
void test_asic_simulation() {
    ESP_LOGI(TAG, "Testing ASIC simulation...");

    GlobalState global_state = {0}; // Minimal initialization for testing

    uint8_t chip_count = ASIC_init(&global_state);
    ESP_LOGI(TAG, "ASIC Test - Detected %d chips", chip_count);

    // Test frequency setting
    ASIC_set_frequency(&global_state, 500.0f);

    // Test version mask
    ASIC_set_version_mask(&global_state, 0x12345678);

    // Test baud rate
    int baud = ASIC_set_max_baud(&global_state);
    ESP_LOGI(TAG, "ASIC Test - Baud rate: %d", baud);

    ESP_LOGI(TAG, "ASIC simulation test completed");
}

/**
 * @brief Test ADC simulation
 */
void test_adc_simulation() {
    ESP_LOGI(TAG, "Testing ADC simulation...");

    ADC_init();

    // Test vcore voltage reading
    uint16_t vcore_voltage = ADC_get_vcore();
    ESP_LOGI(TAG, "ADC Test - Vcore voltage: %d mV", vcore_voltage);

    ESP_LOGI(TAG, "ADC simulation test completed");
}

/**
 * @brief Test ASIC reset simulation
 */
void test_asic_reset_simulation() {
    ESP_LOGI(TAG, "Testing ASIC reset simulation...");

    esp_err_t ret = asic_reset();
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "ASIC reset simulation test passed");
    } else {
        ESP_LOGE(TAG, "ASIC reset simulation test failed");
    }

    ESP_LOGI(TAG, "ASIC reset simulation test completed");
}

/**
 * @brief Test device config simulation
 */
void test_device_config_simulation() {
    ESP_LOGI(TAG, "Testing device config simulation...");

    device_config_sim_init();

    ESP_LOGI(TAG, "Device config simulation test completed");
}

/**
 * @brief Test serial simulation
 */
void test_serial_simulation() {
    ESP_LOGI(TAG, "Testing serial simulation...");

    esp_err_t ret = SERIAL_init();
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Serial init simulation test passed");
    } else {
        ESP_LOGE(TAG, "Serial init simulation test failed");
    }

    // Test baud rate setting
    SERIAL_set_baud(115200);

    // Test buffer operations
    SERIAL_clear_buffer();

    // Test send operation
    uint8_t test_data[] = {0x01, 0x02, 0x03};
    int sent = SERIAL_send(test_data, sizeof(test_data), false);
    ESP_LOGI(TAG, "Serial Test - Sent %d bytes", sent);

    // Test receive operation (should return 0 in simulation)
    uint8_t rx_buf[10];
    int16_t received = SERIAL_rx(rx_buf, sizeof(rx_buf), 100);
    ESP_LOGI(TAG, "Serial Test - Received %d bytes", received);

    // Test debug operations
    SERIAL_debug_rx();

    ESP_LOGI(TAG, "Serial simulation test completed");
}

/**
 * @brief Run all simulation tests
 */
void run_simulation_tests() {
    ESP_LOGI(TAG, "Starting simulation module tests...");

    test_emc2101_simulation();
    test_ina260_simulation();
    test_tps546_simulation();
    test_ds4432u_simulation();
    test_emc2103_simulation();
    test_asic_simulation();
    test_adc_simulation();
    test_asic_reset_simulation();
    test_device_config_simulation();
    test_serial_simulation();

    ESP_LOGI(TAG, "All simulation tests completed");
}