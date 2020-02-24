/**
 * @file OPENLOG.c
 * @author David Hudson
 * @brief 
 * @date 2020-01-29
 * 
 * 
 */

#include "OPENLOG.h"
#include "string.h"



/**
 * @brief Install the uart driver and allocate memory for the openlog buffer
 * 
 * @param openlog_dev 
 */
void OPENLOG_Begin(OPENLOG_STRUCT * openlog_dev)
{
    gpio_pad_select_gpio(DTR_PIN_NUM);
    gpio_set_direction(DTR_PIN_NUM, GPIO_MODE_OUTPUT);

    UART_CONFIG(&openlog_dev->uart_dev, 9600, UART2_TX_PIN_NUM, UART2_RX_PIN_NUM, UART_NUM_2);

    openlog_dev->buffer = (char*) malloc(BUFFER_SIZE);

    UART_START_DEVICE(&openlog_dev->uart_dev);

    gpio_set_level(DTR_PIN_NUM, 0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(DTR_PIN_NUM, 1);

}


void OPENLOG_RESET()
{
    gpio_set_level(DTR_PIN_NUM, 0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(DTR_PIN_NUM, 1);

}


/**
 * @brief Flush UART device communicating with the OpenLog
 * 
 * @param openlog_dev 
 */
void OPENLOG_UART_FLUSH(OPENLOG_STRUCT * openlog_dev)
{
    uart_flush(openlog_dev->uart_dev.port);
}


/**
 * @brief Enter Command Mode
 * 
 * @param openlog_dev 
 */
void OPENLOG_EnterCommandMode(OPENLOG_STRUCT * openlog_dev)
{
    // Default to enter command mode is 3 escape characters
    // A carriage return is sent to get accept other commands.
    char escape[] = {26,26,26,13};

    UART_Write_Bytes(&openlog_dev->uart_dev,  (uint8_t *) escape, 4);
    
    // TODO Confirm that device is in command mode
    /*
    UART_Read_Bytes(&openlog_dev, openlog_dev->buffer, 3);

    if (openlog_dev->buffer[2] == '>')
    {
        openlog_dev->mode = COMMAND_MODE;
    }
    */

    openlog_dev->mode = COMMAND_MODE;

}


/**
 * @brief While in Command Mode enter Append
 *        File Mode to append text to a file
 *        Warning the file name is limited to 12 characters
 * 
 * @param openlog_dev 
 * @param filename 
 */
void OPENLOG_EnterAppendFileMode(OPENLOG_STRUCT * openlog_dev, char * filename)
{

    if (openlog_dev->mode != COMMAND_MODE)
    {
        printf("Not in Command Mode!\n");
        return;
    }

    // Don't append to a file if it's name is larger than 12 characters
    // The OpenLog has 12 character limit on file names.
    if (strlen(filename) > 12) {
        printf("File name to large!!!!\n");
        return;
    }

    sprintf(openlog_dev->buffer, "append %s\r", filename);
    int str_lengh = strlen(openlog_dev->buffer);
    UART_Write_Bytes(&openlog_dev->uart_dev, (uint8_t *)openlog_dev->buffer, str_lengh);
}


/**
 * @brief While in command mode
 *        get a list of available commands
 * 
 * @param openlog_dev 
 */
void OPENLOG_ListCommands(OPENLOG_STRUCT * openlog_dev)
{
    if (openlog_dev->mode == COMMAND_MODE)
    {
        char command[] = "?\r";
        UART_Write_Bytes(&openlog_dev->uart_dev, (uint8_t *) command, 2);
    }
}


/**
 * @brief While in command mode create a new file
 *        Warning the file name is limited to 12 characters
 * @param openlog_dev 
 * @param filename 
 */
void OPENLOG_CreateFile(OPENLOG_STRUCT * openlog_dev, char * filename)
{
    if (openlog_dev->mode != COMMAND_MODE)
    {
        printf("Not in Command Mode!\n");
        return;
    }

    // Don't create a file if it's name is larger than 12 characters
    // The OpenLog has 12 character limit on file names.
    if (strlen(filename) > 12) {
        printf("File name to large!!!!\n");
        return;
    }

    sprintf(openlog_dev->buffer, "new %s\r", filename);
    int str_length = strlen(openlog_dev->buffer);
    UART_Write_Bytes(&openlog_dev->uart_dev, (uint8_t *) openlog_dev->buffer, str_length);

}


/**
 * @brief While in Append File Mode write lines of text to the opened file.
 * 
 * @param openlog_dev 
 * @param filename 
 */
void OPENLOG_WriteLineToFile(OPENLOG_STRUCT * openlog_dev, char * line)
{
    int str_length = strlen(line); // get the number of characters in the string

    // Write the line to the OpenLog device.
    UART_Write_Bytes(&openlog_dev->uart_dev, (uint8_t * ) line, str_length);

}




