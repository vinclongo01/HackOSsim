## How to use FreeRTOS
First we need to compile the FreeRTOS source code and link it with the code of our demo application.

The main **FreeRTOS** functions are contained in the following files (contained in the `FreeRTOS/FreeRTOS/Source`):
- tasks.c
- queue.c
- list.c

Moreover, the processor requires some specific RTOS code for the architecture. This part is located in the `FreeRTOS/FreeRTOS/Source/portable/[compiler]/[architecture]` directory. In this case the __compiler__ is **GCC** (**GNU Compiler Collection**) and the __architecture__ is **ARM_CM3** which refers to the **ARM Cortex-M3 processor**.

The files which handle the **memory management** are located in the `FreeRTOS/FreeRTOS/Source/portable/MemMang` and are called heap_x.c. Typically `heap_4.c` is used. In our project, a revised version of `heap_4.c` will be proposed and tested in the [Memory Management](./demos/memory_management.md) section.    

Another important file for correctly using **FreeRTOS** is the `FreeRTOSCOnfig.h` header file, which contains all the __configurations options__ of the **RTOS**.

## Sections
This project is divided into three separate sections, whose each one will describe a main topic of FreeRTOS by means of some demo applications:
1. [Task Management](./demos/task_management.md)
2. [Queue and Tasks' Synchronization](./demos/queue_and_synchronization.md)
3. [Memory Management](./demos/memory_management.md)