#ifndef EMC2101_SIM_H
#define EMC2101_SIM_H

#include <esp_err.h>
#include "EMC2101.h"

// Function prototypes for EMC2101 simulation
esp_err_t EMC2101_init();
esp_err_t EMC2101_set_ideality_factor(uint8_t ideality);
esp_err_t EMC2101_set_beta_compensation(uint8_t beta);
esp_err_t EMC2101_set_fan_speed(float percent);
uint16_t EMC2101_get_fan_speed(void);
float EMC2101_get_external_temp(void);
float EMC2101_get_internal_temp(void);

#endif // EMC2101_SIM_H