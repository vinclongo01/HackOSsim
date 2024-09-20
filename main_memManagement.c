/*
 * FreeRTOS V202212.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/*
 *******************************************************************************
 * This project provides some demo applications.
 *
 *******************************************************************************
 * This demo application aims to compare different 
 * Memory Heap Allocation Algorithm.
 * 
 * The different Allocation Algorithms are Firts-Fit, Best-Fit and Worst-Fit.
 * The Allocation Type can be changed in the FreeRTOSConfig.h file
 * by setting the value of configHEAP_ALLOCATION_TYPE to:
 * 1 -> Best-Fit
 * 2 -> Worst-Fit
 * any else -> First-Fit
 * 
 *
 * 
 * 
 * 
 * The main:
 * - allocates 1000 bytes
 * - allocates 1000 bytes
 * - allocates 1500 bytes
 * - allocates 100 bytes
 * - allocates 100 bytes
 * - allocates 100 bytes
 * - allocates 100 bytes
 * - deallocates 1000 bytes (second block)
 * - deallocates 100 bytes (fourth block)
 * - deallocates 100 bytes (fifth block)
 * - deallocates 100 bytes (sixth block)
 * - allocates 300 bytes
 * - allocates 1000 bytes
 * - creates a task (TASK 1)
 * 
 * After each operation, the free heap space and the minimum ever free heap space.
 * When a block cannot be allocated, the system will print an error message
 * defined in the vApplicationMallocFailedHook function in main.c.
 * 
 * 
 *
 *
 *******************************************************************************
 * This file only contains the source code that is specific to the full demo.
 * Generic functions, such FreeRTOS hook functions, are defined in main.c.
 *******************************************************************************
 */

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* Demo app includes. */

/*-----------------------------------------------------------*/

/* Task priorities: both with the same priority */
#define mainTASK1_PRIORITY (tskIDLE_PRIORITY + 1)

/* Frequency of each task. The period times are expressed in ticks
by using the pdMS_TO_TICKS() macro. */
#define mainTASK1_PERIOD_MS pdMS_TO_TICKS(20000UL)

/* constant for crude delay implementation */
#define mainDELAY_LOOP_COUNT 10000000UL

/* Tasks' function */
static void vTaskFunction(void *pvParameters);



/* Stack sizes are defined relative to configMINIMAL_STACK_SIZE so they scale
across projects that have that constant set differently - in this case the
constant is different depending on the compiler in use. */
#define mainMESSAGE_BUFFER_STACK_SIZE (configMINIMAL_STACK_SIZE + (configMINIMAL_STACK_SIZE >> 1))
#define mainCHECK_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE + (configMINIMAL_STACK_SIZE >> 1))
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

#include <stdio.h>

void checkHeapUsage(const char *msg, int header_printed)
{
    size_t xFreeHeapSpace = xPortGetFreeHeapSize();
    size_t xMinimumEverFreeHeapSpace = xPortGetMinimumEverFreeHeapSize(); // non implementata in heap2

    // Print header
    if (!header_printed)
    {
        printf("%-30s | %-15s | %-30s\n", "Message", "Free Heap (bytes)", "Minimum Ever Free Heap (bytes)");
        printf("------------------------------------------------------------------------------\n");
        header_printed = 1;
    }

    // Print heap usage
    printf("%-30s | %-15u | %-30u\n", msg, (unsigned int)xFreeHeapSpace, (unsigned int)xMinimumEverFreeHeapSpace);
}

void main_memManagement()
{

    void *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8, *p9, *p10;

    checkHeapUsage("Before allocating memory blocks", 0);

    // Memory block allocation
    p1 = pvPortMalloc(1000);
    checkHeapUsage("After allocated 1000 bytes", 1);

    p2 = pvPortMalloc(1000);
    checkHeapUsage("After allocated 1000 bytes", 1);

    p3 = pvPortMalloc(1500);
    checkHeapUsage("After allocated 1500 bytes", 1);

    p4 = pvPortMalloc(100);
    checkHeapUsage("After allocated 100 bytes", 1);

    p5 = pvPortMalloc(100);
    checkHeapUsage("After allocated 100 bytes", 1);

    p6 = pvPortMalloc(100);
    checkHeapUsage("After allocated 100 bytes", 1);

    p7 = pvPortMalloc(100);
    checkHeapUsage("After allocated 100 bytes", 1);

    vPortFree(p2);
    checkHeapUsage("After deallocated the second block (1000 bytes)", 1);

    vPortFree(p4);
    checkHeapUsage("After deallocated the fourth block (100 bytes)", 1);

    vPortFree(p5);
    checkHeapUsage("After deallocated the fifth block (100 bytes)", 1);

    vPortFree(p6);
    checkHeapUsage("After deallocated the sixth block (100 bytes)", 1);

    p2 = pvPortMalloc(300);
    checkHeapUsage("After allocated 300 bytes", 1);

    p4 = pvPortMalloc(1000);
    checkHeapUsage("After allocated 1000 bytes", 1);

    /*Start the tasks*/
    xTaskCreate(vTaskFunction,                                      /* The function that implements the task. */
                "Task 1",                                           /* The text name assigned to the task - for debug only as it is not used by the kernel. */
                configMINIMAL_STACK_SIZE,                           /* The size of the stack to allocate to the task. */
                (void *)"Hi I'm TASK 1 and I'm about to dead!\r\n", /* The parameter passed to the task, works in a stack fashion */
                mainTASK1_PRIORITY,                                 /* The priority assigned to the task. */
                NULL);                                              /* The task handle is not required, so NULL is passed. */

    checkHeapUsage("After TASK 1 creation", 1);

    // Inizia il scheduler
    vTaskStartScheduler();

    // Non dovrebbe mai arrivare qui
    for (;;);
}

/*-----------------------------------------------------------*/
/* TASK FUNCTION WITH vTaskDelayUntil */
static void vTaskFunction(void *pvParameters)
{
    /*The string to print out passed via task parameters -> TO CAST*/
    const char *pcTaskMsg;
    pcTaskMsg = (char *)pvParameters;
    volatile uint32_t ul; /* volatile to ensure ul is not optimized away. */

    for (;;)
    {
        printf("%s\n", pcTaskMsg);

        /* Delay for a period. */
        vTaskDelayUntil(&ul, mainTASK1_PERIOD_MS);

        // The NULL parameter passed to the vTaskDelete() API function indicates that the task to be deleted is the calling one.
        vTaskDelete(NULL);
    }
}

/*----------------------------------------------------------*/
