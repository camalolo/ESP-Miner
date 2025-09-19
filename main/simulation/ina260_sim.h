#ifndef INA260_SIM_H
#define INA260_SIM_H

#include <esp_err.h>
#include <stdbool.h>
#include "INA260.h"

// Function prototypes for INA260 simulation
esp_err_t INA260_init(void);
bool INA260_installed(void);
float INA260_read_current(void);
float INA260_read_voltage(void);
float INA260_read_power(void);

#endif // INA260_SIM_H