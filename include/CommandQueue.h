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
    CommandQueue();
    void Send(COMMAND_MESSAGE msg);
    bool Recieve(COMMAND_MESSAGE *msg);

private:
    QueueHandle_t commandQueue;
};
