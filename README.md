# How to start

1. Clone the **FreeRTOS** repository from [here](https://www.freertos.org/a00104.html)
It can be easily cloned from github. 

```bash
git clone https://github.com/FreeRTOS/FreeRTOS.git
```

2. Clone this project repository in the same way.
3. Put the project repository folder in the `/FreeRTOS/FreeRTOS/Demo/` directory.
4. Make sure that the DEMO_PROJECT variable in the `/buid/gcc/Makefile`  has the right folder name of the repository. By default it's `DEMO_PROJECT = $(DEMO_ROOT)/HackOSsim` 
5. Follow the instructions for [installation](./docs/installation.md) and [demos](./docs/demos.md).


