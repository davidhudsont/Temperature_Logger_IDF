/**
 * @file bspConsole.c
 * @author David Hudson
 * @brief 
 * @date 2020-02-22
 * 
 */

#include "bspConsole.h"


void Start_Console()
{
    // Drain stdout before reconfiguring it
    fflush(stdout);
    fsync(fileno(stdout));

    // Disable buffering on stdin
    setvbuf(stdin, NULL, _IONBF, 0);

    // Minicom, screen, idf_monitor send CR when ENTER key is pressed
    esp_vfs_dev_uart_set_rx_line_endings(ESP_LINE_ENDINGS_CR);
    // Move the caret to the beginning of the next line on '\n'
    esp_vfs_dev_uart_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);

    // Configure the UART Port for the ESP32 Console 
    uart_config_t uart_config;
    uart_config.baud_rate = CONFIG_CONSOLE_UART_BAUDRATE;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_1;

    // Install the uart driver
    uart_driver_install(CONFIG_CONSOLE_UART_NUM, 256, 0, 0, NULL, 0);
    uart_param_config(CONFIG_CONSOLE_UART_NUM, &uart_config);

    // Tell VFS to use UART Driver
    esp_vfs_dev_uart_use_driver(CONFIG_CONSOLE_UART_NUM);
     
    // Configure the console
    esp_console_config_t console_config;
    console_config.max_cmdline_length = 256;
    console_config.max_cmdline_args = 8;
    console_config.hint_color = atoi(LOG_COLOR_CYAN);

    // Initiailize the console
    esp_console_init(&console_config);
    
    // Configure linenoise line completion lbirary
    linenoiseSetMultiLine(1);

    // Tell linenoise where to get command completions and hints
    linenoiseSetCompletionCallback(&esp_console_get_completion);
    linenoiseSetHintsCallback((linenoiseHintsCallback*) &esp_console_get_hint);

    // Set command history size
    linenoiseHistorySetMaxLen(100);

}

// Register the consoles commands after the console is started
void Register_Console_Commands()
{
    esp_console_register_help_command();
    register_system();
}
