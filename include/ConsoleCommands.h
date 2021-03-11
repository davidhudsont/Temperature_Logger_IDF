#pragma once

typedef enum COMMAND_ID
{
    COMMAND_NULL = -1,
    COMMAND_SET_SECONDS = 0,
    COMMAND_SET_MINUTES,
    COMMAND_SET_12HOURS,
    COMMAND_SET_24HOURS,
    COMMAND_SET_WEEKDAY,
    COMMAND_SET_DATE,
    COMMAND_SET_MONTH,
    COMMAND_SET_YEAR,
    COMMAND_GET_DATETIME,
    COMMAND_GET_TEMPF,
    COMMAND_GET_TEMPC,
    COMMAND_GET_DISK,
    COMMAND_WRITE_DISK,
    COMMAND_START_LOG,
    COMMAND_STOP_LOG,
    COMMAND_DELETE_LOG,
    COMMAND_LCD_DISPLAY_OFF,
    COMMAND_LCD_DISPLAY_ON,
    COMMAND_LCD_SET_CONTRAST,
    COMMAND_LCD_SET_BACKLIGHT,
    COMMAND_LCD_CLEAR_DISPLAY,
} COMMAND_ID;

typedef struct COMMAND_MESSAGE_STRUCT
{
    COMMAND_ID id;
    int arg1;
    int arg2;
    int arg3;
} COMMAND_MESSAGE_STRUCT;

int recieve_rtc_command(COMMAND_MESSAGE_STRUCT *msg);
int recieve_tmp_command(COMMAND_MESSAGE_STRUCT *msg);
int recieve_sdcard_command(COMMAND_MESSAGE_STRUCT *msg);
int recieve_lcd_command(COMMAND_MESSAGE_STRUCT *msg);

/**
 * @brief Register all the console commands
 * 
 */
void register_system(void);

/**
 * @brief Create the console command queues
 */
void register_queues(void);
