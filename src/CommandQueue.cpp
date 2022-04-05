#include "CommandQueue.h"

CommandQueue::CommandQueue()
{
    commandQueue = xQueueCreate(3, sizeof(COMMAND_MESSAGE));
}

void CommandQueue::Send(int id, int arg1, int arg2, int arg3)
{
    COMMAND_MESSAGE msg;
    msg.id = id;
    msg.arg1 = arg1;
    msg.arg2 = arg2;
    msg.arg3 = arg3;

    xQueueSend(commandQueue, (void *)&msg, 30);
}

bool CommandQueue::Recieve(COMMAND_MESSAGE *msg)
{
    return xQueueReceive(commandQueue, msg, 30);
}
