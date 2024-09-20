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
 * This demo application aims to show how FreeRTOS scheduler works by changing 
 * its configurations, analysing the behaviour of two tasks:
 * - TASK 2 (PRIORITY = 1) creates a second task TASK 1 (PRIORITY = 2 at the beginning), then enters in 
 *   an infinite loop wherein it prints a message
 * - TASK 1: it changes its priority, first making it equal to TASK 2's one, then incrementing it again, and so on ...
 * BOTH the tasks implement a CRUDE DELAY with a for which consume CPU time (i.e., the tasks are not
 * moved into the waiting list).
 * 
 * 
 * CHANGING THE SCHEDULER CONFIGURATIONS THE FOLLOWING BEHAVIOUR IS OBTAINED:
 * - configUSE_PREEMPTION = 1 and configUSE_TIME_SLICING = 0 -> TASK 2 creates TASK 1 which preempts TASK 2 and continues to run 
 * - configUSE_PREEMPTION = 1 and configUSE_TIME_SLICING = 1 -> TASK 2 creates TASK 1 which preempts TASK 2, when TASK 1 raises its 
 * 																priority down the two tasks alternate each other. At a certain point
 * 																TASK 1 increments again its priority continuing to run, and so on...
 * - configUSE_PREEMPTION = 0 and configUSE_TIME_SLICING = x -> the first task that enters in the running state, i.e. TASK 2, continues running.
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
#define mainTASK1_PRIORITY (tskIDLE_PRIORITY + 2)
#define mainTASK2_PRIORITY (tskIDLE_PRIORITY + 1)


/* constant for crude delay implementation */
#define mainDELAY_LOOP_COUNT 10000000UL
#define TICK_THRESHOLD 1000  // threshold for incrementing priority

/* Tasks' function */
static void vTask1(void *pvParameters);
static void vTask2(void *pvParameters);

/* Tasks' messages */
static const char *pcTask1Msg = "Task 1 is running\r\n";
static const char *pcTask2Msg = "Task 2 is running\r\n";

/* Stack sizes are defined relative to configMINIMAL_STACK_SIZE so they scale
across projects that have that constant set differently - in this case the
constant is different depending on the compiler in use. */
#define mainMESSAGE_BUFFER_STACK_SIZE (configMINIMAL_STACK_SIZE + (configMINIMAL_STACK_SIZE >> 1))
#define mainCHECK_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE + (configMINIMAL_STACK_SIZE >> 1))
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

void main_priority(void)
{

	/*Start the tasks*/
	xTaskCreate(vTask2,			  /* The function that implements the task. */
				"Task 2",				  /* The text name assigned to the task - for debug only as it is not used by the kernel. */
				configMINIMAL_STACK_SIZE, /* The size of the stack to allocate to the task. */
				(void *)pcTask2Msg,		  /* The parameter passed to the task, works in a stack fashion */
				mainTASK2_PRIORITY,		  /* The priority assigned to the task. */
				NULL);					  /* The task handle is not required, so NULL is passed. */





	/* Start the scheduler. */
	vTaskStartScheduler();

	/* If configSUPPORT_STATIC_ALLOCATION was false then execution would only
	get here if there was insufficient heap memory to create either the idle or
	timer tasks.  As static allocation is used execution should never be able
	to reach here. */
	for (;;);

}

/*-----------------------------------------------------------*/
/* TASK FUNCTION WITH vTaskDelayUntil */
static void vTask1(void *pvParameters)
{
	/*The string to print out passed via task parameters -> TO CAST*/
	const char *pcTaskMsg;
	pcTaskMsg = (char *)pvParameters;

	// the counter implementing the crude delay
	volatile uint32_t ul; /* volatile to ensure ul is not optimized away. */


	TickType_t xStartTime, xCurrentTime;
	/* The xStartTime variable needs to be initialized with the current tick
    count. Note that this is the only time the variable is written to explicitly.
    After this xStartTime is automatically updated within vTaskDelayUntil(). */
	xStartTime = xTaskGetTickCount();

	TickType_t xLastWakeTime, xTimeIncrement;
	xLastWakeTime = xTaskGetTickCount();
	/* The xTimeIncrement variable is used to implement a task that executes
	periodically and with a fixed frequency. */
	xTimeIncrement = pdMS_TO_TICKS(5000);
	//

	// PRIORITY HANDLING VARIABLES
	UBaseType_t uxPriority;
	volatile uint32_t equal_priorities = 0; // flag: PRIORITY CHANGED
	// get current priority
    uxPriority = uxTaskPriorityGet(NULL);


	for (;;)
	{
		xCurrentTime = xTaskGetTickCount();
		printf("%s\n", pcTaskMsg);

		/** FOR DEBUGGING */
		// printf("%d\n", xCurrentTime);

		
		if ((xCurrentTime - xStartTime) >= TICK_THRESHOLD && equal_priorities)
        {
            // Make the TASK 1 priority bigger
            uxPriority++;
            
            printf("Task 1 priority incremented to %d\n", uxPriority);

			vTaskPrioritySet(NULL, uxPriority);

            // set flag to 0 -> TASK 1 has bigger priority now
            equal_priorities = 0;
			// reset the start time
			xStartTime = xTaskGetTickCount();
		
        }

		if ((xCurrentTime - xStartTime) >= TICK_THRESHOLD && !equal_priorities)
        {
            // Make the two priorities equal
            uxPriority--;
            
            printf("Task 1 priority decremented to %d\n", uxPriority);

			vTaskPrioritySet(NULL, uxPriority);

            // set flag to 1 -> the two tasks have the same priority now (equal to 1)
            equal_priorities = 1;
			// reset the start time
			xStartTime = xTaskGetTickCount();
		
        }

		/* Place this task in the blocked state until it is time to run again.
		The block time is specified in ticks, pdMS_TO_TICKS() was used to
		convert a time specified in milliseconds into a time specified in ticks.
		While in the Blocked state this task will not consume any CPU time. */
		
		/** DELAY IMPLEMENTATION -> in this way the task is not moved to the waiting list
		 * it's possible to observe the round robin behaviour
		 */
		// vTaskDelayUntil( &xLastWakeTime, xTimeIncrement );

		 
		// CRUDE DELAY IMPLEMENTATION 
		for (ul = 0; ul < mainDELAY_LOOP_COUNT; ul++)
		{
			//This loop is just a very crude delay implementation. There is
			//nothing to do in here. Later examples will replace this crude
			//loop with a proper delay/sleep function. 
			// In this way the task is not moved to the waiting list.
		}
		


		
	}
	printf("Task 1 is going to be deleted\n");
	// The NULL parameter passed to the vTaskDelete() API function indicates that the task to be deleted is the calling one.
	vTaskDelete( NULL );
}
/*-----------------------------------------------------------*/


static void vTask2(void *pvParameters)
{

	printf("Task 2 about to create Task 1\n");

	// TASK 2 creates TASK 1 whose priority is bigger
	xTaskCreate(vTask1, "Task 1", configMINIMAL_STACK_SIZE, (void *)pcTask1Msg, mainTASK1_PRIORITY, NULL);


	/*The string to print out passed via task parameters -> TO CAST*/
	const char *pcTaskMsg;
	pcTaskMsg = (char *)pvParameters;
	volatile uint32_t ul; /* volatile to ensure ul is not optimized away. */


	TickType_t xLastWakeTime, xTimeIncrement;
	/* The xLastWakeTime variable needs to be initialized with the current tick
   	count. Note that this is the only time the variable is written to explicitly.
   	After this xLastWakeTime is automatically updated within vTaskDelayUntil(). */
	xLastWakeTime = xTaskGetTickCount();

	/* The xTimeIncrement variable is used to implement a task that executes
	periodically and with a fixed frequency. */
	xTimeIncrement = pdMS_TO_TICKS(5000);


	for (;;)
	{
		printf("%s\n", pcTaskMsg);

		/* Place this task in the blocked state until it is time to run again.
		The block time is specified in ticks, pdMS_TO_TICKS() was used to
		convert a time specified in milliseconds into a time specified in ticks.
		While in the Blocked state this task will not consume any CPU time. */
		
		/** DELAY IMPLEMENTATION -> in this way the task is not moved to the waiting list
		 * it's possible to observe the round robin behaviour
		 */
		// vTaskDelayUntil( &xLastWakeTime, xTimeIncrement );


		
		// CRUDE DELAY IMPLEMENTATION 
		for (ul = 0; ul < mainDELAY_LOOP_COUNT; ul++)
		{
			//This loop is just a very crude delay implementation. There is
			//nothing to do in here. Later examples will replace this crude
			//loop with a proper delay/sleep function. 
			// In this way the task is not moved to the waiting list.
		}
		

	}
	printf("Task 2 is going to be deleted\n");
	// The NULL parameter passed to the vTaskDelete() API function indicates that the task to be deleted is the calling one.
	vTaskDelete( NULL );
}


