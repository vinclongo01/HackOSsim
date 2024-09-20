# Using FreeRTOS with QEMU

_Go back to the [main page](../README.md)_

This section provides a tutorial to set up a Real Time OS (FreeRTOS) on QEMU. 
In this tutorial is possible to find detailed instructions for the intsallation using both Windows and Fedora as operating system on a 64 bit system.  

- [Installation](#installation)
  - [Windows](#Windows)
  - [Fedora](#Fedora)
  - [Usage](#usage)
    - [QEMU](#qemu)
    - [FreeRTOS](#freertos)
    - [Debugging FreeRTOS with QEMU without VSCode](#debugging-freertos-with-qemu-without-vscode)
    - [Debugging FreeRTOS with QEMU in VSCode](#debugging-freertos-with-qemu-in-vscode)


## Installation

The following applications and tools need to be installed:

- **VSCode (Visual Studio Code)**: one of the most famous and used open-source code editor designed by Microsoft. 

- **QEMU**: an open-source machine emulator that allows you to virtualize hardware types including ARM processors, enabling development and testing of ARM software without needing actual physical hardware.

- **ARM GNU Toolchain**: this package includes tools such that compilers, assemblers, linkers, and debuggers to create bare-metal software for ARM processors. In this case, we are interested in creating 32-bit ARM CPU bare-metal software. 

- **CMake**: a cross-platform build system generator that helps in the build process for complicated applications.

- **Git**: a distributed version control system that helps in code development, version management, collaboration and sharing of code by tracking changes made to your codebase.

- **make**: a build automation tool that allows to reads `Makefile` scripts for crontolling the build process specifying how to link and compile your application

### Windows
This section provides the instructions for setting up the system on **Windows 11**.

1. **VSCode** can be easily installed [from here](https://code.visualstudio.com/download). No detailed tutorial is provided in this case because many comphrensive ones are already available online.
2. **QEMU**'s binaries for Windows can be found at the following link
    - [Windows 64 bit](https://qemu.weilnetz.de/w64/)
3. **ARM GNU Toolchain**'s binaries can be found at the following link [Arm GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads). In this instance follow the `AArch32 bare-metal target (arm-none-eabi)` for `Windows (mingw-w64-i686) hosted cross toolchains`.
4. **CMake**'s binaries can be found at the following link [CMake](https://cmake.org/download/). Once downloaded the `zip` file, find the executable to install the package.
5. For Windows, download and install **Git** (including Bash terminal) [from here](https://git-scm.com/downloads).
6. On Windows you can install **make** using `chocolatey` (install it from [here](https://chocolatey.org/), and add it to the PATH environment variable - [here for instructions](https://www.computerhope.com/issues/ch000549.htm)) running this command from the terminal `> choco install make`

Now, add the QEMU, Arm GNU Compiler, CMake, and `make` installation paths to your `PATH` environment variable, ([follow here for instructions](https://www.computerhope.com/issues/ch000549.htm)).

### Fedora
This section provides the instructions for setting up the system on **Fedora Linux 40**.

First of all update all the package repositories and installed packages on Fedora running the following command:

```bash
sudo dnf upgrade --refresh
```

1. **VSCode** can be easily installed by running the following commands:

Add the Microsoft repositories to your system:

```bash
sudo rpm --import https://packages.microsoft.com/keys/microsoft.asc
```

Next, add the Visual Studio Code Repository:

```bash
sudo sh -c 'echo -e "[code]\nname=Visual Studio Code\nbaseurl=https://packages.microsoft.com/yumrepos/vscode\nenabled=1\ngpgcheck=1\ngpgkey=https://packages.microsoft.com/keys/microsoft.asc" > /etc/yum.repos.d/vscode.repo'
```

Finally, install VSCode by running:

```bash
sudo dnf install code
```

2. **QEMU** can be easily installed by following the instructions [here](https://www.qemu.org/download/#linux). On Fedora running the following command is enough
```bash
dnf install @virtualization
```
The binaries will be automatically downloaded in the `/usr/bin` directory. `/usr/bin` is a standard directory in the system's `PATH` environment variable, meaning that any executable placed here can be run from the command line without needing to specify its full path.

3. **ARM GNU Toolchain** can be installed in the following way

For Linux download the `.tar.xz` file in the _AArch32 bare-metal target (arm-none-eabi)_ section [here](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads).

Extract the toolchain in a chosen directory (for example `/opt/`)**
```bash
sudo tar -xvf arm-gnu-toolchain-<VERSION>-x86_64-arm-none-eabi.tar.xz -C /opt/
```
Set up symbolic link in `/usr/bin` to point to the extracted version of the compiler tools:

```bash
sudo ln -s /opt/arm-gnu-toolchain-<VERSION>-x86_64-arm-none-eabi/bin/arm-* /usr/bin/
```
Alternatively you can manually add the path to the `PATH` environment `PATH` by modifying the `~/.bashrc` file.

Check if the installation was successful by inspecting the versions:

```bash
arm-none-eabi-gcc --version
```

4. **CMake** can be installed from [here](https://cmake.org/download/)
For Linux download the `.tar.gz` file for your architexture.

First, extract the CMake binaries in the `tar.gz` file to a directory of your choice (again `/opt/`).
```bash
sudo tar -xzvf cmake-3.30.2-linux-x86_64.tar.gz -C /opt/
```
Set up symbolic link in `/usr/bin` to point to the extracted binaries.
```bash
sudo ln -s /opt/cmake-3.30.2-linux-x86_64/bin/cmake /usr/bin/
```
Check if the installation was successful by inspecting the versions:

```bash
cmake --version
```

5. **Git** should already be installed on Linux-based systems.

6. **make** can be easily installed in the following way
```bash
sudo dnf install make
```
The binaries will be automatically downloaded in the `/usr/bin` directory. For this reason no symbolic link needs to be set up.

------------------------------------------------------------------

## Usage

### QEMU

This section provides a comphrensive tutorial on how to use the `QEMU` virtualizer to emulate an embedded system.

QEMU allows to virtualize lots of hardware systems by offering lots of packages `qemu-system-<target_arch>`. In this instance we are intersted in emulating a 32-bit ARM architecture, so the command to use is `qemu-system-arm`

The options to provide are:

- **-machine name**: the target machine to emulate. Run `qemu-system-arm -machine help` to get a list of all the available machines.

- **-cpu cpu**: specify the cpu to be used. Run `qemu-system-arm -cpu help` to get a list of all the available cpus

- **-kernel bzImage**: to provide the executable to be run as kernel image

Other useful options are:

- **-serial dev**: redirect the serial port to the device 'dev'

- **-monitor dev**: redirect the monitor to char device 'dev'

- **-S**: freeze CPU at startup

- **-gdb dev**: accept gdb connection on 'dev'

- **-s**: short end for `-gdb tcp::1234`

- **-semihosting**: semihosting mode

- **-semihosting-config [enable=on|off][,target=native|gdb|auto][,chardev=id][,userspace=on|off][,arg=str[,...]]**: semihosting configuration

### FreeRTOS
FreeRTOS is the real time operating system for embedded systems chosen for this project. Download instructions and API references can be found at its official [website](https://www.freertos.org/a00104.html). It can be easily cloned from github. 

```bash
git clone https://github.com/FreeRTOS/FreeRTOS.git
```

### Debugging FreeRTOS with QEMU without VSCode

In the FreeRTOS' repo a Demo Project is already available for understanding how to emulate the embedded OS on QEMU.

This demo can be found in the folder `FreeRTOS/FreeRTOS/Demo/CORTEX_MPS2_QEMU_IAR_GCC`.

#### Building the Demo

To build the demo run the next command in the `FreeRTOS/FreeRTOS/Demo/CORTEX_MPS2_QEMU_IAR_GCC/build/gcc` directory.

```bash
make
```

A successful build creates the elf file `FreeRTOS/FreeRTOS/Demo/CORTEX_MPS2_QEMU_IAR_GCC/build/gcc/output/RTOSDemo.out`.

#### Running the FreeRTOS Demo on QEMU

Now QEMU can be started by running the following command


```bash
qemu-system-arm -machine mps2-an385 -cpu cortex-m3 -kernel [path-to]/RTOSDemo.out -monitor none -nographic -serial stdio -s -S
```
_Note_: replace `[path-to]` with the path to the `RTOSDemo.out` file generated by the GCC build.

The options `-s -S` run QEMU attaching the debugger. Omit them if you don't need it.

#### Debugging with GDB

To start a command line debug session it's possible to use the `arm-none-eabi-gdb` tool from the `RTOSDemo.out`'s path running the following command:

```bash
arm-none-eabi-gdb RTOSDemo.out -ex "target remote localhost:1234"
```

### Debugging FreeRTOS with QEMU in VSCode

Open the terminal in the Demo Project folder `FreeRTOS/FreeRTOS/Demo/CORTEX_MPS2_QEMU_IAR_GCC` and open VSCode by running `code .`
After VScode loads the demo folder, open `.vscode/launch.json` in the editor. Find the `miDebuggerPath` parameter and change the value to the path where `arm-none-eabi-gdb` is located on your machine.

Then open `main.c` and make sure that `mainCREATE_SIMPLE_BLINKY_DEMO_ONLY`is set to 1. This will generate only the simple blinky demo.

By pressing the `Run and Debug` button from the left side panel in VSCode it's possible to select `Launch QEMU RTOSDemo`. This will build the code, run the program, and attach the debugger.

At this point a little menu will be opened at the top of the editor where you can choose if `Continue`, `Step Over`, `Step Into`, `Step Out`, or `Stop`. You can also add breakpoints in the code by right clicking next to the line number.

Pressing the `Continue` button will execute the code, which will produce the output in the Terminal.

Now we can use this example to play a bit and explore FreeRTOS.
  
