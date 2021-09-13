// Standard Libraries
#include <stdio.h>

// RTOS
#include "driver/uart.h"
#include "esp_console.h"
#include "esp_log.h"
#include "esp_vfs_dev.h"
#include "linenoise/linenoise.h"
#include "sdkconfig.h"

// User Headers
#include "BSPConsole.h"
#include "ConsoleCommands.h"

void StartConsole()
{
    // Drain stdout before reconfiguring it
    fflush(stdout);
    fsync(fileno(stdout));

    // Disable buffering on stdin
    setvbuf(stdin, NULL, _IONBF, 0);

    // Minicom, screen, idf_monitor send CR when ENTER key is pressed
    esp_vfs_dev_uart_port_set_rx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM, ESP_LINE_ENDINGS_CR);
    // Move the caret to the beginning of the next line on '\n'
    esp_vfs_dev_uart_port_set_tx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM, ESP_LINE_ENDINGS_CRLF);

    // Configure the UART Port for the ESP32 Console
    uart_config_t uart_config;
    uart_config.baud_rate = CONFIG_ESP_CONSOLE_UART_BAUDRATE;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.source_clk = UART_SCLK_REF_TICK;

    // Install the uart driver
    ESP_ERROR_CHECK(uart_driver_install(CONFIG_ESP_CONSOLE_UART_NUM, 256, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(CONFIG_ESP_CONSOLE_UART_NUM, &uart_config));

    // Tell VFS to use UART Driver
    esp_vfs_dev_uart_use_driver(CONFIG_ESP_CONSOLE_UART_NUM);

    // Configure the console
    esp_console_config_t console_config;
    console_config.max_cmdline_length = 256;
    console_config.max_cmdline_args = 8;
    console_config.hint_color = atoi(LOG_COLOR_CYAN);

    // Initiailize the console
    ESP_ERROR_CHECK(esp_console_init(&console_config));

    // Configure linenoise line completion lbirary
    linenoiseSetMultiLine(1);

    // Tell linenoise where to get command completions and hints
    linenoiseSetCompletionCallback(&esp_console_get_completion);
    linenoiseSetHintsCallback((linenoiseHintsCallback *)&esp_console_get_hint);

    // Set command history size
    linenoiseHistorySetMaxLen(100);

    // Don't return empty lines
    linenoiseAllowEmpty(false);

    // Register the consoles commands after the console is started
    esp_console_register_help_command();
    register_console_commands();
}
