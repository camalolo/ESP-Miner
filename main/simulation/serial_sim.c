#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "serial_sim";

esp_err_t SERIAL_init(void) {
    ESP_LOGI(TAG, "SERIAL SIMULATION: Skipping UART initialization");
    return ESP_OK;
}

esp_err_t SERIAL_set_baud(int baud) {
    ESP_LOGI(TAG, "SERIAL SIMULATION: Skipping baud rate change to %i", baud);
    return ESP_OK;
}

void SERIAL_clear_buffer(void) {
    ESP_LOGI(TAG, "SERIAL SIMULATION: Skipping buffer clear");
}

int SERIAL_send(uint8_t *data, int len, bool debug) {
    ESP_LOGI(TAG, "SERIAL SIMULATION: Skipping send of %d bytes", len);
    return len; // Pretend it sent
}

int16_t SERIAL_rx(uint8_t *buf, uint16_t size, uint16_t timeout_ms) {
    ESP_LOGI(TAG, "SERIAL SIMULATION: Skipping receive");
    return 0; // No data
}

void SERIAL_debug_rx(void) {
    ESP_LOGI(TAG, "SERIAL SIMULATION: Skipping debug rx");
}