#ifndef EMC2103_SIM_H
#define EMC2103_SIM_H

#include <esp_err.h>
#include "EMC2103.h"

// Function prototypes for EMC2103 simulation
esp_err_t EMC2103_init();
esp_err_t EMC2103_set_ideality_factor(uint8_t ideality);
esp_err_t EMC2103_set_beta_compensation(uint8_t beta);
esp_err_t EMC2103_set_fan_speed(float percent);
uint16_t EMC2103_get_fan_speed(void);
float EMC2103_get_external_temp(void);
float EMC2103_get_external_temp2(void);

#endif // EMC2103_SIM_H