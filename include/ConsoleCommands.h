#pragma once

#define DISABLE_SD_CARD
#ifndef DISABLE_SD_CARD
int recieve_sdcard_command(COMMAND_MESSAGE_STRUCT *msg);
#endif

/**
 * @brief Register all the console commands
 * 
 */
void register_system(void);

/**
 * @brief Create the console command queues
 */
void register_queues(void);
