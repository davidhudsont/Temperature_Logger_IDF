# Warning! This code was automatically generated for projects
# without default 'CMakeLists.txt' file.

set(app_sources
	"ConsoleCommands.c"
	"DEADONRTC.c"
	"OPENLOG.c"
	"TMP102.c"
	"bspConsole.c"
	"bspI2C.c"
	"bspSpi.c"
	"bspUART.c"
	"main.c"
)

idf_component_register(SRCS ${app_sources})
