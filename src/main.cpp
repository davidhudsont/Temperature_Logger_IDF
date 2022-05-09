/**
 * @author David Hudson
 * @brief Main Application for Clock Display
 *
 */

#ifdef NATIVE

int main()
{
    return 0;
}

#else

// User Headers
#include "SystemTasks.h"

extern "C"
{
    void app_main(void);
}

// cppcheck-suppress unusedFunction
void app_main()
{
    CreateTasks();
}

#endif
