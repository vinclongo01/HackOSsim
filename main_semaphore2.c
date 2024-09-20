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
 * This demo application shows the main FreeRTOS API functions for Tasks Synchronization
 * using sempahores.
 * Semaphores are objects used to send interrupts for unblocking tasks.
 * This results in tasks synchronization with interrupts.
 * 
 * The main creates the queue, semaphores and four tasks with the SAME PRIORITY:
 * - three PRODUCERS which send items in the queue
 * - one CONSUMER which reads from the queue
 *
 * Totally there are four semaphores:
 * - three binary semaphores for the three producers
 * - one counting semaphore for the consumer
 * 
 * Each PRODUCER inserts its value in the queue, increases the counting semaphore
 * of the CONSUMER and unblock the next task (PRODUCER1 -> PRODUCER2 -> PRODUCER3 -> CONSUMER)
 * The general behaviour is the following:
 * the producer inserts its value and unblock the next task. Then the consumer reads
 * out all the values inserted by the producer before unblocking the producers.
 * 
 * Also here the priority does not change the behaviour of the tasks.
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
#include "semphr.h"

/* Demo app includes. */

/*-----------------------------------------------------------*/

/* Task priorities: both with the same priority */
#define mainPRODUCER_PRIORITY (tskIDLE_PRIORITY + 1)
#define mainCONSUMER_PRIORITY (tskIDLE_PRIORITY + 1)

/* Frequency of each task. The period times are expressed in ticks
by using the pdMS_TO_TICKS() macro. */
#define mainTASK1_PERIOD_MS pdMS_TO_TICKS(20000UL)
#define mainTASK2_PERIOD_MS pdMS_TO_TICKS(30000UL)
#define mainTIMER_PERIOD_MS pdMS_TO_TICKS(1200000UL)

/* constant for crude delay implementation */
#define mainDELAY_LOOP_COUNT 10000000UL

/* Tasks' function */
// static void vTaskFunction(void *pvParameters);

/* Stack sizes are defined relative to configMINIMAL_STACK_SIZE so they scale
across projects that have that constant set differently - in this case the
constant is different depending on the compiler in use. */
#define mainMESSAGE_BUFFER_STACK_SIZE (configMINIMAL_STACK_SIZE + (configMINIMAL_STACK_SIZE >> 1))
#define mainCHECK_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE + (configMINIMAL_STACK_SIZE >> 1))
/*-----------------------------------------------------------*/

static QueueHandle_t xQueue2;
static SemaphoreHandle_t xSemaphoreProducer1;
static SemaphoreHandle_t xSemaphoreProducer2;
static SemaphoreHandle_t xSemaphoreProducer3;
static SemaphoreHandle_t xSemaphoreConsumerCounting;

/*-----------------------------------------------------------*/

// Producer Task Function
static void vProducerTask1(void *pvParameters)
{
    int itemToSend;
    BaseType_t xStatus;
    const TickType_t xTicksToWait = pdMS_TO_TICKS(2000); 

    itemToSend = (int)pvParameters;
    /* infinite loop. */
    for (;;)
    {

        if (xSemaphoreTake(xSemaphoreProducer1, portMAX_DELAY) == pdTRUE)
        {
            xStatus = xQueueSendToBack(xQueue2, &itemToSend, xTicksToWait);

            if (xStatus != pdPASS)
            {
                /** It should never get here */
                printf("The queue is full. %d\r\n", itemToSend);
                xSemaphoreGive(xSemaphoreConsumerCounting);
            }
            else
            {
                printf("------------------------------------------------------\n");
                printf("Producer 1 sent: %d\n", itemToSend);
                printf("------------------------------------------------------\n");
                xSemaphoreGive(xSemaphoreProducer2);
                xSemaphoreGive(xSemaphoreConsumerCounting);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// Producer Task Function
static void vProducerTask2(void *pvParameters)
{
    int itemToSend;
    BaseType_t xStatus;
    const TickType_t xTicksToWait = pdMS_TO_TICKS(2000); 

    itemToSend = (int)pvParameters;
    /* infinite loop. */
    for (;;)
    {

        if (xSemaphoreTake(xSemaphoreProducer2, portMAX_DELAY) == pdTRUE)
        {
            xStatus = xQueueSendToBack(xQueue2, &itemToSend, xTicksToWait);

            if (xStatus != pdPASS)
            {
                /** It should never get here */
                printf("The queue is full. %d\r\n", itemToSend);
                xSemaphoreGive(xSemaphoreConsumerCounting);
            }
            else
            {
                printf("------------------------------------------------------\n");
                printf("Producer 2 sent: %d\n", itemToSend);
                printf("------------------------------------------------------\n");
                xSemaphoreGive(xSemaphoreProducer3);
                xSemaphoreGive(xSemaphoreConsumerCounting);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// Producer Task Function
static void vProducerTask3(void *pvParameters)
{
    int itemToSend;
    BaseType_t xStatus;
    const TickType_t xTicksToWait = pdMS_TO_TICKS(2000); 

    itemToSend = (int)pvParameters;
    /* infinite loop. */
    for (;;)
    {

        if (xSemaphoreTake(xSemaphoreProducer3, portMAX_DELAY) == pdTRUE)
        {
            xStatus = xQueueSendToBack(xQueue2, &itemToSend, xTicksToWait);

            if (xStatus != pdPASS)
            {
                /** It should never get here */
                printf("The queue is full. %d\r\n", itemToSend);
                xSemaphoreGive(xSemaphoreConsumerCounting);
            }
            else
            {
                printf("------------------------------------------------------\n");
                printf("Producer 3 sent: %d\n", itemToSend);
                printf("------------------------------------------------------\n");
                xSemaphoreGive(xSemaphoreConsumerCounting);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// Consumer Task Function
static void vConsumerTask()
{

    int receivedItem;
    BaseType_t xStatus;

    /* This task is also defined within an infinite loop. */
    for (;;)
    {
        /* Receive from the queue.
        The second parameter is the buffer into which the received data will be
        placed.  In this case the buffer is simply the address of a variable that
        has the required size to hold the received structure.

        The last parameter is the block time - the maximum amount of time that the
        task will remain in the Blocked state to wait for data to be available
        if the queue is already empty.  In this case a block time is not necessary
        because this task will only run when the queue is full. */

        while (xSemaphoreTake(xSemaphoreConsumerCounting, portMAX_DELAY) == pdTRUE)
        {
            xStatus = xQueueReceive(xQueue2, &receivedItem, 0);

            if (xStatus == pdPASS)
            {
                /* Data was successfully received from the queue, print out the received
                value and the source of the value. */
                printf("------------------------------------------------------\n");
                printf("Consumer received: %d\n", receivedItem);
                printf("------------------------------------------------------\n");
            }
            else
            {
                /* Nothing was received from the queue.  This must be an error as this
                task should only run when the queue is full. */
                printf("Now the queue is empty\n");
                break;
            }
            xSemaphoreGive(xSemaphoreProducer1);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void main_semaphore2()
{

    // Queue creation
    xQueue2 = xQueueCreate(10, sizeof(int));
    
    // Semaphore creation
    xSemaphoreProducer1 = xSemaphoreCreateBinary();
    xSemaphoreProducer2 = xSemaphoreCreateBinary();
    xSemaphoreProducer3 = xSemaphoreCreateBinary();
    /** Use a counting semaphore for the consumer to read the three items sent by the producers each time */
    xSemaphoreConsumerCounting = xSemaphoreCreateCounting(3, 0);

    if (xQueue2 != NULL)
    {
        // Tasks' creation
        xTaskCreate(vProducerTask1, "Producer1", configMINIMAL_STACK_SIZE, (void *)10, 1, NULL);
        xTaskCreate(vProducerTask2, "Producer2", configMINIMAL_STACK_SIZE, (void *)20, 1, NULL); 
        xTaskCreate(vProducerTask3, "Producer3", configMINIMAL_STACK_SIZE, (void *)30, 1, NULL);

        xTaskCreate(vConsumerTask, "Consumer", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

        // Enable semaphore for the first producer
        xSemaphoreGive(xSemaphoreProducer1);

        // Start the scheduler
        vTaskStartScheduler();
    }
    else
    {
        printf("Error in creating the queue\n");
    }

    // If the control reaches here, the scheduler has stopped and the system is in an error state
    for (;;)
        ;
}