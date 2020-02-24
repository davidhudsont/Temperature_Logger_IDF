/**
 * @file OPENLOG.h
 * @author David Hudson 
 * @brief Simplified OpenLog driver.
 * @date 2020-01-29
 * 
 * 
 */

#ifndef _OPEN_LOG_H_
#define _OPEN_LOG_H_

#include "bspUART.h"
#include "driver/gpio.h"


#define DTR_PIN_NUM (GPIO_NUM_4)


/**
 * @brief OpenLog operating Modes
 * 
 */
typedef enum OPENLOG_MODES{
    NEW_FILE_LOGGING = 0,
    APPEND_FILE_LOGGING,
    COMMAND_MODE

} OPENLOG_MODES;

/**
 * @brief OpenLog device structure
 * 
 */
typedef struct OPENLOG_STRUCT {    
    UART_STRUCT uart_dev;
    char * buffer;
    OPENLOG_MODES mode;

} OPENLOG_STRUCT;

void OPENLOG_Begin(OPENLOG_STRUCT * openlog_dev);

void OPENLOG_RESET();
void OPENLOG_UART_FLUSH(OPENLOG_STRUCT * openlog_dev);

void OPENLOG_EnterCommandMode(OPENLOG_STRUCT * openlog_dev);
void OPENLOG_ListCommands(OPENLOG_STRUCT * openlog_dev);

void OPENLOG_CreateFile(OPENLOG_STRUCT * openlog_dev, char * filename);
void OPENLOG_EnterAppendFileMode(OPENLOG_STRUCT * openlog_dev, char * filename);
void OPENLOG_WriteLineToFile(OPENLOG_STRUCT * openlog_dev, char * line);

#endif