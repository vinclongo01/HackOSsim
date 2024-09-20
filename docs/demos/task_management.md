# Task Management - Demo Applications

_Go back to the [demos page](../demos.md)_

_Go back to the [main page](../../README.md)_

This section provides the description of some demo applications which depict the main FreeRTOS functionalities concerning **Task Management**. 
In each tutorial is possible to find detailed instructions for running each **DEMO application** in VSCode.  

- [Demo Applications Structure](#demo-applications-structure)
  - [Simple Example with CRUDE DELAY](#simple-example-with-crude-delay)
  - [Simple Example with DELAY/SLEEP FUNCTION](#simple-example-with-delaysleep-function)
  - [Dynamic Priority](#dynamic-priority)



## Demo Applications Structure
Each DEMO application in this project is selected in the `main` by setting the `mainCREATE_SIMPLE_DEMO` value.
Considering the **Task Management** we have that
- `mainCREATE_SIMPLE_DEMO = 1` selects the `main_three_tasks_CRUDE.c` DEMO application, i.e, the simple example with CRUDE DELAY.
- `mainCREATE_SIMPLE_DEMO = 2` selects the `main_three_tasks.c` DEMO application, i.e, the simple example with DELAY/SLEEP FUNCTION. 
- `mainCREATE_SIMPLE_DEMO = 3` selects the `main_priority.c` DEMO application, i.e., the one with dynamic priority. 

Now open `VSCode` for testing the demo applications. Select the demo to test by setting the `mainCREATE_SIMPLE_DEMO` variable to the proper value.
Then from the __Run and Debug__ section click on __Launch QEMU RTOSDemo__ to run the application attaching the debugger.

### Simple Example with CRUDE DELAY
The `main_three_tasks_CRUDE.c` application is a simple example which shows the main **FreeRTOS API functions** for **creating**, **developing** and **managing tasks** in an embedded system.
The application simply consists of **three tasks**. All of them have the same priority and each one implements the same function which **prints a message** and enters in a loop with the unique functionality to delay the task (a **CRUDE DELAY**, i.e. which does not move the task in the waiting list). 

The general behaviour of the three tasks changes by modifying the scheduler configurations in the `FreeRTOSConfig.h` file. 
Totally there are three possibilities:
- `configUSE_PREEMPTION = 1` and `configUSE_TIME_SLICING = 0` -> The scheduler is preemtive, priority-based with no time slicing (i.e., ROUND ROBIN DISABLED). When a task with high priority enters in the ready state preempts the running task (with lower priority). Tasks with same priority does not share CPU time (i.e., no round robin behaviour).

    In this case, since all the three tasks have the same priority, the first one that enters in the running state, takes precedence and continues to run.

- `configUSE_PREEMPTION = 1` and `configUSE_TIME_SLICING = 1` -> The scheduler is preemptive, priority-based with time slicing enabled (i.e., ROUND ROBIN LIKE). Tasks with higher priority preempts task with lower one. When one or more tasks have the same priority of the running one, they share CPU time by alternating each other when a time slice occurs. 

    In this case the three tasks alternate each other in an unpredictable way.

- `configUSE_PREEMPTION = 0` and `configUSE_TIME_SLICING = x` -> The scheduler is not preemptive. This means that context switchings happen only when the running task explicitly moves to a **NOT RUNNING STATE** (suspended, blocked, or ended).

    In this case, since all the tasks have the same priority, the first task that enters in the running state, continues to run.


### Simple Example with DELAY/SLEEP FUNCTION
The `main_three_tasks.c` application is identical to the previous one. The unique difference consists in the implementation of the delay. In this case the API function `vTaskDelayUntil()` offered by FreeRTOS is used to delay the task. This function just moves the task in the `blocked state`, making room for tasks in `ready state`.

In this case the three tasks alternate each other with any configuration of the scheduler.


### Dynamic Priority
The `main_priority.c` application is a more advanced example which aims to show how FreeRTOS scheduler works by analysing the behaviour of two tasks where one of them has dynamic priority. 
The demo application consists of two tasks:
- the **TASK 2** with lower priority (PRIORITY = 1) creates the second task **TASK 1** (at the beginning with PRIORITY = 2), then enters in an infinite loop wherein it prints a message.
- the **TASK 1** which continously changes its priority, first making it equal to **TASK 2**'s one, then increasing it again and so on...

**BOTH** the tasks implement a **CRUDE DELAY** with a for which consume CPU time (i.e., the tasks are not moved into the waiting list).

The general behaviour of the two tasks changes by modifying the scheduler configurations in the `FreeRTOSConfig.h` file. 
Totally there are three possibilities:
- `configUSE_PREEMPTION = 1` and `configUSE_TIME_SLICING = 0` -> As already seen before, with this configurations, the scheduler is preemtive, priority-based with no time slicing (i.e., ROUND ROBIN DISABLED). When a task with high priority enters in the ready state preempts the running task (with lower priority). Tasks with same priority does not share CPU time (i.e., no round robin behaviour).

    In this case, **TASK 2** creates **TASK 1** which preempts **TASK 2** and continues to run.

- `configUSE_PREEMPTION = 1` and `configUSE_TIME_SLICING = 1` -> As already mentioned before, in this case, the scheduler is preemptive, priority-based with time slicing enabled (i.e., ROUND ROBIN LIKE). Tasks with higher priority preempts task with lower one. When one or more tasks have the same priority of the running one, they share CPU time by alternating each other a time slice occurs. 

    In this case, the **TASK 2** creates **TASK 1** which preempts **TASK 2**, when **TASK 1** decreases its priority the two tasks alternate each other. At a certain point **TASK 1** increases again its priority continuing to run, and so on...

- `configUSE_PREEMPTION = 0` and `configUSE_TIME_SLICING = x` -> With this configurations, the scheduler is not preemptive. This means that context switchings happen only when the task explicitly moves to a **NOT RUNNING STATE** (suspended, blocked, or ended).

    In this case, the first task that enters in the running state, i.e. **TASK 2**, continues running.

