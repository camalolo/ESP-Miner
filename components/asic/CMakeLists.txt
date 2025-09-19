set(SRCS_LIST
    "bm1370.c"
    "bm1368.c"
    "bm1366.c"
    "bm1397.c"
    "crc.c"
    "common.c"
    "frequency_transition_bmXX.c"
    "pll.c"
)

if(CONFIG_RUN_ON_DEV_BOARD)
    list(APPEND SRCS_LIST "../../main/simulation/serial_sim.c" "../../main/simulation/asic_sim.c")
else()
    list(APPEND SRCS_LIST "serial.c" "asic.c")
endif()

idf_component_register(
SRCS ${SRCS_LIST}

INCLUDE_DIRS 
    "include"

REQUIRES 
    "freertos"
    "driver"
    "stratum"
)


# Include the header files from "main" directory
target_include_directories(${COMPONENT_LIB} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../main")

# Include the header files from "main/tasks" directory
target_include_directories(${COMPONENT_LIB} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../main/tasks")
