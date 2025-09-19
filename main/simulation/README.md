# ESP-Miner Hardware Simulation

This directory contains simulation modules for ESP-Miner hardware components, allowing the firmware to run on an ESP32S3 dev board without physical hardware.

## Overview

When `CONFIG_RUN_ON_DEV_BOARD=y` is set in `sdkconfig.defaults`, the firmware will use simulated hardware components instead of real ones. This enables:

- Full firmware testing and development on dev boards
- Realistic sensor data and hardware behavior simulation
- Easier debugging and feature development
- Continuous integration testing

The simulation includes 10 hardware components: temperature sensors, power monitoring, voltage regulation, digital potentiometer, ASIC mining, ADC measurements, ASIC reset, device configuration, and serial communication.

## Simulated Components

### EMC2101 Temperature Sensor
- **File**: `emc2101_sim.c`
- **Simulates**: External and internal temperature readings, fan speed control
- **Behavior**: Returns random temperatures (25-45°C) with realistic variation

### EMC2103 Temperature Sensor
- **File**: `emc2103_sim.c`
- **Simulates**: Dual external temperature readings, fan speed control
- **Behavior**: Returns random temperatures (35-55°C) with realistic variation

### INA260 Power Monitor
- **File**: `ina260_sim.c`
- **Simulates**: Voltage, current, and power measurements
- **Behavior**: Returns stable readings around 12V/2.5A/30W with minor variation

### TPS546 Voltage Regulator
- **File**: `tps546_sim.c`
- **Simulates**: Input/output voltage, current, temperature, and control
- **Behavior**: Returns stable voltages around configured values with realistic variation

### DS4432U Digital Potentiometer
- **File**: `ds4432u_sim.c`
- **Simulates**: Current code setting and voltage control
- **Behavior**: Accepts configuration inputs and simulates stable outputs

### ASIC Mining Chip
- **File**: `asic_sim.c`
- **Simulates**: Chip initialization, work submission, and result processing
- **Behavior**: Returns simulated mining results with realistic timing

### ADC (Analog-to-Digital Converter)
- **File**: `adc_sim.c`
- **Simulates**: Vcore voltage measurements
- **Behavior**: Returns voltage readings based on configured ASIC voltage with small random variations

### ASIC Reset
- **File**: `asic_reset_sim.c`
- **Simulates**: Hardware reset operations for ASIC chips
- **Behavior**: Returns success status for reset operations

### Device Configuration
- **File**: `device_config_sim.c`
- **Simulates**: NVS (Non-Volatile Storage) initialization with device defaults
- **Behavior**: Sets up simulation defaults for device model, ASIC model, board version, and component enablement

### Serial/UART Communication
- **File**: `serial_sim.c`
- **Simulates**: UART communication for ASIC chip interaction
- **Behavior**: Simulates serial operations (init, send, receive, baud rate changes) without actual hardware communication

## Configuration

### Enabling Simulation Mode

1. Set `CONFIG_RUN_ON_DEV_BOARD=y` in `sdkconfig.defaults`
2. Rebuild the firmware
3. The firmware will automatically use simulation modules

### Current Implementation

The simulation is controlled by a single flag (`CONFIG_RUN_ON_DEV_BOARD`) that enables all component simulations. Individual component simulation control can be added if needed.

## Usage

### Building with Simulation

```bash
# Enable dev board mode
echo "CONFIG_RUN_ON_DEV_BOARD=y" >> sdkconfig.defaults

# Build the firmware
idf.py build
```

### Testing Simulations

A comprehensive test file `test_simulations.c` is provided to verify all 10 simulation modules:

```c
#include "simulation/test_simulations.h"

void app_main() {
    run_simulation_tests();
}
```

The test suite covers EMC2101/2103 sensors, INA260 power monitor, TPS546 regulator, DS4432U potentiometer, ASIC mining, ADC measurements, ASIC reset, device configuration, and serial communication.

### Expected Behavior

When simulation mode is active:

- All log messages will indicate "SIMULATION" mode
- Hardware components return plausible simulated values
- ASIC mining produces simulated results
- Temperature readings vary realistically over time
- Power measurements show stable values with minor fluctuations

## Implementation Details

### API Compatibility

All simulation modules provide the exact same API as their real hardware counterparts, ensuring transparent integration:

- Same function signatures
- Same return types
- Same error codes
- Same behavior patterns

### Randomization

Simulations use ESP32's hardware random number generator for realistic variation:

- Temperature readings drift slowly over time
- Power measurements have minor random fluctuations
- ASIC results are generated probabilistically

### Logging

Simulation modules include comprehensive logging:

- **ESP_LOGI**: Initialization and major events
- **ESP_LOGD**: Detailed value reporting (can be enabled for debugging)

## Development Notes

### Adding New Simulations

To add simulation for a new hardware component:

1. Analyze the real hardware driver's API
2. Create a simulation module with identical function signatures
3. Implement realistic simulated behavior
4. Add conditional compilation in initialization code
5. Update this documentation

### Testing

- Individual simulation modules can be tested using `test_simulations.c`
- Full integration testing requires building with `CONFIG_RUN_ON_DEV_BOARD=y`
- Monitor logs for "SIMULATION" indicators

## Troubleshooting

### Common Issues

1. **Simulation not activating**: Ensure `CONFIG_RUN_ON_DEV_BOARD=y` in sdkconfig
2. **Build errors**: Check that simulation header files are included correctly
3. **Unexpected values**: Verify random seed initialization in simulation modules

### Debug Logging

Enable debug logging to see detailed simulation values:

```c
// In ESP-IDF monitor
idf.py monitor
```

Look for "SIMULATION" in log output to verify simulation mode is active.