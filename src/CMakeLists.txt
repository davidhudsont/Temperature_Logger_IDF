# Warning! This code was automatically generated for projects
# without default 'CMakeLists.txt' file.

set(app_sources
    "BSP_I2C.cpp"
    "BSP_SPI.cpp"
    "TMP102.cpp"
    "DEADONRTC.cpp"
    "System_Tasks.cpp"
    "bspConsole.cpp"
    "ConsoleCommands.cpp"
    "OPENLOG.c"
    "bspUART.c"
    "main.cpp"
)

idf_component_register(SRCS ${app_sources})
