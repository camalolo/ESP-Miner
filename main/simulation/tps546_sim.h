#ifndef TPS546_SIM_H
#define TPS546_SIM_H

#include <esp_err.h>
#include "TPS546.h"
#include "global_state.h"

// Function prototypes for TPS546 simulation
esp_err_t TPS546_init(TPS546_CONFIG config);
esp_err_t TPS546_clear_faults(void);
void TPS546_read_mfr_info(uint8_t *read_mfr_revision);
void TPS546_write_entire_config(void);
int TPS546_get_frequency(void);
void TPS546_set_frequency(int newfreq);
int TPS546_get_temperature(void);
float TPS546_get_vin(void);
float TPS546_get_iout(void);
float TPS546_get_vout(void);
esp_err_t TPS546_set_vout(float volts);
void TPS546_show_voltage_settings(void);
esp_err_t TPS546_check_status(GlobalState * GLOBAL_STATE);
const char* TPS546_get_error_message(void);

#endif // TPS546_SIM_H