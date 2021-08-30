#include "CommandQueue.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"

CommandQueue::CommandQueue()
{
    commandQueue = xQueueCreate(3, sizeof(COMMAND_MESSAGE));
}
void CommandQueue::Send(COMMAND_MESSAGE msg)
{
    xQueueSend(commandQueue, (void *)&msg, 30);
}

bool CommandQueue::Recieve(COMMAND_MESSAGE *msg)
{
    return xQueueReceive(commandQueue, msg, 30);
}