#ifndef DS4432U_SIM_H
#define DS4432U_SIM_H

#include <esp_err.h>
#include "DS4432U.h"

// Function prototypes for DS4432U simulation
esp_err_t DS4432U_init(void);
esp_err_t DS4432U_set_current_code(uint8_t output, uint8_t code);
esp_err_t DS4432U_set_voltage(float vout);
esp_err_t DS4432U_get_current_code(uint8_t output, uint8_t *code);
esp_err_t DS4432U_test(void);

#endif // DS4432U_SIM_H