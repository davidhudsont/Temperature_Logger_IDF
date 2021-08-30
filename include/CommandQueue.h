#pragma once

#include "stdint.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

struct COMMAND_MESSAGE
{
    int id;
    int arg1;
    int arg2;
    int arg3;
};

class CommandQueue
{
public:
    CommandQueue()
    {
        commandQueue = xQueueCreate(3, sizeof(COMMAND_MESSAGE));
    }
    void Send(COMMAND_MESSAGE *msg)
    {
        xQueueSend(commandQueue, (void *)&msg, 30);
    }

    bool Recieve(COMMAND_MESSAGE *msg)
    {
        return xQueueReceive(commandQueue, msg, 30);
    }

private:
    QueueHandle_t commandQueue;
};
