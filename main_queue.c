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
 * This demo application shows the main FreeRTOS API functions for Queue Management.
 * Queues provide Task-to-Task communication mechanism.
 *
 * The main creates four tasks:
 * - three PRODUCERS which send items in the queue
 * - one CONSUMER which reads from the queue (with the highest priority)
 *
 * When the scheduler starts the CONSUMER task tries to read from the queue which
 * is empty. The CONSUMER task will block for 100ms waiting for data to be available.
 * At this point one of the PRODUCERS will send data to the queue, unblocking the CONSUMER task 
 * which will read the data from the queue. The CONSUMER task will then block again waiting for data 
 * and the cycle will continue.
 * In this way the PRODUCERS and CONSUMER tasks are synchronized by the queue.
 * Sometimes if the 100 ms elapses while the queue is still empty, the CONSUMER task will print an error message. 
 * Moreover, each task contains a CRUDE DELAY implementation for demonstration purposes.
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

/* Frequency of each task. The period times are expressed in ticks
by using the pdMS_TO_TICKS() macro. */
#define mainTASK1_PERIOD_MS pdMS_TO_TICKS(20000UL)
#define mainTASK2_PERIOD_MS pdMS_TO_TICKS(30000UL)
#define mainTIMER_PERIOD_MS pdMS_TO_TICKS(1200000UL)

/* constant for crude delay implementation */
#define mainDELAY_LOOP_COUNT 10000000UL

/* Task priorities */
#define mainPRODUCER_PRIORITY (tskIDLE_PRIORITY + 1)
#define mainCONSUMER_PRIORITY (tskIDLE_PRIORITY + 2)

/* Tasks' function */
static void vProducerTask(void *pvParameters);
static void vConsumerTask(void *pvParameters);

/* Stack sizes are defined relative to configMINIMAL_STACK_SIZE so they scale
across projects that have that constant set differently - in this case the
constant is different depending on the compiler in use. */
#define mainMESSAGE_BUFFER_STACK_SIZE (configMINIMAL_STACK_SIZE + (configMINIMAL_STACK_SIZE >> 1))
#define mainCHECK_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE + (configMINIMAL_STACK_SIZE >> 1))
/*-----------------------------------------------------------*/

QueueHandle_t xQueue;

/*-----------------------------------------------------------*/

// Producer Task Function
static void vProducerTask(void *pvParameters)
{
    volatile uint32_t ul; /* volatile to ensure ul is not optimized away. */
    int32_t lValueToSend;
    BaseType_t xStatus;

    /* Three instances of this task are created so the value that is sent to the
    queue is passed in via the task parameter - this way each instance can use
    a different value, particularly:
    - 100 for PRODUCER 1
    - 200 for PRODUCER 2
    - 300 for PRODUCER 3
    The queue was created to hold values of type int32_t, so cast the parameter to the required type. */

    lValueToSend = (int32_t)pvParameters;
    /* As per most tasks, this task is implemented within an infinite loop. */
    for (;;)
    {
        /* Send the value to the queue.
        The first parameter is the queue to which data is being sent. The
        queue was created before the scheduler was started, so before this task
        started to execute.
        The second parameter is the address of the data to be sent, in this case
        the address of lValueToSend.
        The third parameter is the Block time – the time the task should be kept
        in the Blocked state to wait for space to become available on the queue
        should the queue already be full. In this case a block time is not
        specified because the queue should never contain more than one item, and
        therefore never be full. */
        printf("----------------------------------------------\n");
        printf("Producer %d about to send %d to the queue\n", lValueToSend / 100, lValueToSend);
        printf("----------------------------------------------\n");
        xStatus = xQueueSendToBack(xQueue, &lValueToSend, 0);
        if (xStatus != pdPASS)
        {
            /* The send operation could not complete because the queue was full -
            this must be an error as the queue should never contain more than
            one item! */
            printf("----------------------------------------------\n");
            printf("Could not send to the queue.\r\n");
            printf("----------------------------------------------\n");
        }

        // CRUDE DELAY IMPLEMENTATION
        for (ul = 0; ul < mainDELAY_LOOP_COUNT; ul++)
        {
            // This loop is just a very crude delay implementation. There is
            // nothing to do in here. Later examples will replace this crude
            // loop with a proper delay/sleep function.
        }

    }
}

// Consumer Task Function
static void vConsumerTask(void *pvParameters)
{
    /* Declare the variable that will hold the values received from the queue. */
    volatile uint32_t ul; /* volatile to ensure ul is not optimized away. */
    int32_t lReceivedValue;
    BaseType_t xStatus;
    const TickType_t xTicksToWait = pdMS_TO_TICKS(100);
    /* This task is also defined within an infinite loop. */
    for (;;)
    {
        /* This call should always find the queue empty because this task will
        immediately remove any data that is written to the queue. */
        if (uxQueueMessagesWaiting(xQueue) != 0)
        {
            printf("----------------------------------------------\n");
            printf("Queue should have been empty!\r\n");
            printf("----------------------------------------------\n");
        }
        else {
            printf("----------------------------------------------\n");
            printf("Queue is empty - waiting for data...\n");
            printf("----------------------------------------------\n");
        }
        /* Receive data from the queue.
        The first parameter is the queue from which data is to be received. The
        queue is created before the scheduler is started, and therefore before this
        task runs for the first time.
        The second parameter is the buffer into which the received data will be
        placed. In this case the buffer is simply the address of a variable that
        has the required size to hold the received data.
        The last parameter is the block time – the maximum amount of time that the
        task will remain in the Blocked state to wait for data to be available
        should the queue already be empty. */
        xStatus = xQueueReceive(xQueue, &lReceivedValue, xTicksToWait);
        if (xStatus == pdPASS)
        {
            /* Data was successfully received from the queue, print out the received
            value. */
            printf("----------------------------------------------\n");
            printf("Consumer Received = %d \n", lReceivedValue);
            printf("----------------------------------------------\n");
        }
        else
        {
            /* Data was not received from the queue even after waiting for 100ms.
            This must be an error as the sending tasks are free running and will be
            continuously writing to the queue. */
            printf("----------------------------------------------\n");
            printf("Could not receive from the queue.\r\n");
            printf("----------------------------------------------\n");
        }

        // CRUDE DELAY IMPLEMENTATION
        for (ul = 0; ul < mainDELAY_LOOP_COUNT; ul++)
        {
            // This loop is just a very crude delay implementation. There is
            // nothing to do in here. Later examples will replace this crude
            // loop with a proper delay/sleep function.
        }

    }
}

void main_queue()
{
    // queue creation
    xQueue = xQueueCreate(5, sizeof(int32_t));
    printf("Queue created\n");

    if (xQueue != NULL)
    {
        // Create three instances of the producer task
        xTaskCreate(vProducerTask, "Producer1", configMINIMAL_STACK_SIZE, (void *)100, mainPRODUCER_PRIORITY, NULL);
        xTaskCreate(vProducerTask, "Producer2", configMINIMAL_STACK_SIZE, (void *)200, mainPRODUCER_PRIORITY, NULL);
        xTaskCreate(vProducerTask, "Producer3", configMINIMAL_STACK_SIZE, (void *)300, mainPRODUCER_PRIORITY, NULL);

        // Create the task that will read from the queue
        xTaskCreate(vConsumerTask, "Consumer", configMINIMAL_STACK_SIZE, NULL, mainCONSUMER_PRIORITY, NULL);

        // Start the scheduler so the tasks start executing.
        vTaskStartScheduler();
    }
    else
    {
        printf("Error in creating queue\n");
    }

    // If all is well we will never reach here as the scheduler will now be running the tasks.
    for (;;);
}