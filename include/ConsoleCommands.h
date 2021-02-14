/**
 * @file ConsoleCommands.h
 * @author David Hudson
 * @brief Header file
 * @date 2020-02-21
 * 
 */

#ifndef __REGISTER_COMMANDS_H_
#define __REGISTER_COMMANDS_H_

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
    COMMAND_STOP_LOG,
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
int recieve_openlog_command(COMMAND_MESSAGE_STRUCT *msg);

void register_system(void);
void register_queues(void);

#endif