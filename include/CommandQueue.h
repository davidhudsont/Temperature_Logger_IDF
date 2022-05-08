#pragma once

// Standard Libraries
#include "stdint.h"

// RTOS
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

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
    void Send(int id, int arg1 = 0, int arg2 = 0, int arg3 = 0);
    bool Recieve(COMMAND_MESSAGE *msg);

private:
    QueueHandle_t commandQueue;
};

class CommandSemaphore
{
public:
    CommandSemaphore();
    void GiveSemaphore();
    bool TakeSemaphore();

private:
    SemaphoreHandle_t commandSemaphore;
};