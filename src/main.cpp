/**
 * @author David Hudson
 * @brief Main Application for Temperature Logger
 * 
 */

#ifdef NATIVE

int main()
{
    return 0;
}

#else

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "System_Tasks.h"

extern "C"
{
    void app_main(void);
}

void app_main()
{
    printf("Starting Tasks!\n");
    Create_Task_Queues();
    Create_Tasks();
}

#endif