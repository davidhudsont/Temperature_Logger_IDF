# Warning! This code was automatically generated for projects
# without default 'CMakeLists.txt' file.

set(app_sources
    "ConsoleCommands.c"
    "DEADONRTC.c"
    "OPENLOG.c"
    "TMP102.cpp"
    "bspConsole.c"
    "bspI2C.c"
    "BSP_I2C.cpp"
    "BSP_SPI.cpp"
    "bspSpi.c"
    "bspUART.c"
    "System_Tasks.cpp"
    "main.cpp"
)

idf_component_register(SRCS ${app_sources})
