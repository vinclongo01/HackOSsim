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
 * This demo application shows the main FreeRTOS API functions for Task Management.
 * The main just creates three tasks with the same priority. 
 * Each task implements the same function which:
 *  - prints a message
 *  - enters in a loop with no further functionalities except than delaying (the CRUDE DELAY)
 * 
 * 
 * CHANGING THE SCHEDULER CONFIGURATIONS THE FOLLOWING BEHAVIOUR IS OBTAINED:
 * - configUSE_PREEMPTION = 1 and configUSE_TIME_SLICING = 0 -> The first task that enters in the running state, continues to run.
 * - configUSE_PREEMPTION = 1 and configUSE_TIME_SLICING = 1 -> The three tasks alternate each other in an unpredictable way.
 * - configUSE_PREEMPTION = 0 and configUSE_TIME_SLICING = x -> The first task that enters in the running state, continues to run.
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

/* Task priorities: all with the same priority */
#define mainTASK1_PRIORITY (tskIDLE_PRIORITY + 1)
#define mainTASK2_PRIORITY (tskIDLE_PRIORITY + 1)
#define mainTASK3_PRIORITY (tskIDLE_PRIORITY + 1)

/* Frequency of each task. The period times are expressed in ticks
by using the pdMS_TO_TICKS() macro. */
#define mainTASK1_PERIOD_MS pdMS_TO_TICKS(20000UL)
#define mainTASK2_PERIOD_MS pdMS_TO_TICKS(30000UL)
#define mainTIMER_PERIOD_MS pdMS_TO_TICKS(1200000UL)

/* constant for crude delay implementation */
#define mainDELAY_LOOP_COUNT 10000000UL

/* Tasks' function */
static void vTaskFunction(void *pvParameters);

/* Tasks' messages */
static const char *pcTask1Msg = "Task 1 is running\r\n";
static const char *pcTask2Msg = "Task 2 is running\r\n";
static const char *pcTask3Msg = "Task 3 is running\r\n";

/* Stack sizes are defined relative to configMINIMAL_STACK_SIZE so they scale
across projects that have that constant set differently - in this case the
constant is different depending on the compiler in use. */
#define mainMESSAGE_BUFFER_STACK_SIZE (configMINIMAL_STACK_SIZE + (configMINIMAL_STACK_SIZE >> 1))
#define mainCHECK_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE + (configMINIMAL_STACK_SIZE >> 1))
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

void main_three_tasks_CRUDE(void)
{

	/*Start the tasks*/
	xTaskCreate(vTaskFunction,			  /* The function that implements the task. */
				"Task 1",				  /* The text name assigned to the task - for debug only as it is not used by the kernel. */
				configMINIMAL_STACK_SIZE, /* The size of the stack to allocate to the task. */
				(void *)pcTask1Msg,		  /* The parameter passed to the task, works in a stack fashion */
				mainTASK1_PRIORITY,		  /* The priority assigned to the task. */
				NULL);					  /* The task handle is not required, so NULL is passed. */

	xTaskCreate(vTaskFunction, "Task 2", configMINIMAL_STACK_SIZE, (void *)pcTask2Msg, mainTASK2_PRIORITY, NULL);
	xTaskCreate(vTaskFunction, "Task 3", configMINIMAL_STACK_SIZE, (void *)pcTask3Msg, mainTASK3_PRIORITY, NULL);

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
static void vTaskFunction(void *pvParameters)
{
	/*The string to print out passed via task parameters -> TO CAST*/
	const char *pcTaskMsg;
	pcTaskMsg = (char *)pvParameters;
	volatile uint32_t ul; /* volatile to ensure ul is not optimized away. */


	for (;;)
	{
		printf("%s\n", pcTaskMsg);

		/* Place this task in the blocked state until it is time to run again.
		The block time is specified in ticks, pdMS_TO_TICKS() was used to
		convert a time specified in milliseconds into a time specified in ticks.
		While in the Blocked state this task will not consume any CPU time. */
		
		
		// CRUDE DELAY IMPLEMENTATION 
		for (ul = 0; ul < mainDELAY_LOOP_COUNT; ul++)
		{
			//This loop is just a very crude delay implementation. There is
			//nothing to do in here. Later examples will replace this crude
			//loop with a proper delay/sleep function. 
		}


	}
	// The NULL parameter passed to the vTaskDelete() API function indicates that the task to be deleted is the calling one.
	vTaskDelete( NULL );
}
/*-----------------------------------------------------------*/
































/*-----------------------------------------------------------*/
/* TASK FUNCTION WITH vTaskDelay */

// static void vTaskFunction( void *pvParameters )
// {

// const TickType_t xDelayTime = pdMS_TO_TICKS( 250 ); // 250 ms
// const char *pcTaskMsg;
// pcTaskMsg = (char *)pvParameters;

// 	for( ;; )
// 	{
//      printf("%s\n",pcTaskMsg);
// 		/* Place this task in the blocked state until it is time to run again.
// 		The block time is specified in ticks, pdMS_TO_TICKS() was used to
// 		convert a time specified in milliseconds into a time specified in ticks.
// 		While in the Blocked state this task will not consume any CPU time. */
// 		vTaskDelay( xDelayTime );
//
//
//  }
// 	// The NULL parameter passed to the vTaskDelete() API function indicates that the task to be deleted is the calling one.
// 	vTaskDelete( NULL );	
// }
/*-----------------------------------------------------------*/


/**
 * 
 *  // CRUDE DELAY IMPLEMENTATION 
		for (ul = 0; ul < mainDELAY_LOOP_COUNT; ul++)
		{
			//This loop is just a very crude delay implementation. There is
			//nothing to do in here. Later examples will replace this crude
			//loop with a proper delay/sleep function. 
		}
 * 
 */