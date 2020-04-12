/**
 * @file main.c
 * @author David Hudson
 * @brief Main Application for Temperature Logger
 * @date 2019-11-15
 * 
 * 
 */
#ifdef NATIVE
    
int main() { return 0;}

#else

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "System_Tasks.h"



void app_main()
{
    printf("Starting Tasks!\n");

    Create_Task_Queues();
    Create_Tasks();

}

#endif