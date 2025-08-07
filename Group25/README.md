# ENCE464 Fitness Monitor Project

This repository contains the code and build system for the ENCE464 Fitness
Monitor project, to run on the
[Texas Instruments TM4C123G LaunchPad Evaluation Kit](https://www.ti.com/tool/EK-TM4C123GXL). 

## Repository structure

The repository is structured as follows:

* `docs`: Instructions for building and running the project code, and installing the
  development toolchain on your own computer. Also contains Fitness Monitor v1.0 
  documentation and some additional project help.
* `labs`: Supporting code for your lab sessions.
* `target`: Code that targets the LaunchPad; contains the main project code,
  libraries, and a few simple demo programs.
* `tests`: Tests targeting the host (development) machine.
* `.vscode`: Visual Studio Code (VS Code) configuration files.

## 🚀 Getting started

The project is set up for development using Microsoft _Visual Studio Code_, the 
_CMake_ build system, the _GCC ARM_ cross-compiler, and _OpenOCD_. 

### Toolchain setup

If you're working on a UC Embedded Systems Lab (ESL) computer then you should find that the 
necessary toolchain is pre-installed. 

If you want to build the project on _your personal machine_ you'll need to make sure that you 
install all of the necessary tools. Check the toolchain installation guide for your operating system:

* [Windows](docs/toolchain.md#windows)
* [Linux](docs/toolchain.md#linux)
* [macOS](docs/toolchain.md#macos)

### Building the code 

Assuming that you have the [toolchain installed](#toolchain), you can use the command line or _VS Code_ to 
build the code and program your LaunchPad. Here are instructions on building/programming using each approach:

* [VS Code](docs/vs-code.md)
* [Command line](docs/cli.md)

See the [CMake guide](docs/cmake.md) for additional info on performing various tasks with
the CMake build system.

For tips on debugging and fixing common issues, including some common FreeRTOS
errors, see the [FAQ](docs/faq.md).

## 🌐 Additional resources

* [Tiva TM4C123GH6PM datasheet (PDF)](https://www.ti.com/lit/ds/symlink/tm4c123gh6pm.pdf)
* [Tiva LaunchPad manual (PDF)](https://www.ti.com/lit/ug/spmu296/spmu296.pdf)
* [Orbit BoosterPack manual (PDF)](https://digilent.com/reference/_media/orbit_boosterpack/orbit_boosterpack_rm.pdf)
* [TivaWare API documentation (PDF)](https://www.ti.com/lit/ug/spmu298e/spmu298e.pdf)
* [FreeRTOS Documentation](https://www.freertos.org/Documentation/RTOS_book.html)
* [Unity test framework documentation](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityGettingStartedGuide.md)
    - [Assertions cheatsheet (PDF)](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityAssertionsCheatSheetSuitableforPrintingandPossiblyFraming.pdf)
* [CMake documentation](https://cmake.org/cmake/help/latest/)
