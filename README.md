<div align="center">
  <h1 style="background: linear-gradient(to right, black, white); -webkit-background-clip: text; -webkit-text-fill-color: transparent; margin: 0;">
        [Pathogen 🦠]
    </h1>
    <br>
    <br>
    <p align="center">
        <img src="https://img.shields.io/github/issues/Raumberg/pathogen?style=for-the-badge">
        <br>
        <img src="https://img.shields.io/github/languages/count/Raumberg/pathogen?style=for-the-badge">
        <img src="https://img.shields.io/github/repo-size/Raumberg/pathogen?style=for-the-badge">
        <br>
    </p>
</div>

## An IOCTL driver for Reading/Writing memory.
This is a simple driver for mainly educational purposes to r/w memory deep in kernel.
I'm fairly bad at C programming (as of now), so consider this as a frankenstein of youtube videos and available github repos.

## 🚀 Features
- **Seek process**: The driver can seek for processes using PsLookupProcessById
- **Safe to use**: No reinterpret_casts of C++, checks for mem buffer length before manipulating.
- **IRP**: Handles IRP requests by user (usermode->kernelmode)

> [!IMPORTANT]
> I'm not using VS since I hate all the "magic" and prefer to write code by myself and compile everything by myself to see what is really going on. 

### So, how to build it?..
1. Install [Windows 10 SDK (10.0.26100.0)](https://developer.microsoft.com/ru-ru/windows/downloads/windows-sdk/) and [Windows Driver Kit](https://learn.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk).
2. Install [Visual Studio](https://visualstudio.microsoft.com/ru/) in order to use Microsoft's "cl" compilator.
3. Ensure the installation of libraries is correct by observing the base path (C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0). km/ and um/ folders need to be present.
4. If you are using VSCode, consider adding installed libs to path (see .vscode config in repo or do it manually).
5. Open the Native Tools Command Prompt for VS and navigate to the project's folder.
6. Use the command in Makefile to build the driver.
7. Rename the driver.exe to driver.sys
8. Reboot windows in debug mode (with driver signatures check disabled)
9. Create service and start service of the driver.
10. Perform your IOCTL operation by building an interface to pass IRP requests.

Compiling a driver (explained):
```bash
cl /O2 driver.c "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\km\x64\ntoskrnl.lib" /I "C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\km" /link /subsystem:native /driver:wdm -entry:DriverEntry
# cl - native Microsoft's compiler
# /O2 - compiler's optimization mode
# C:\...\km\x64\ntoskrnl.lib - windows kernel library linkage
# /I C:\...\Include\<version>\km - kernel C headers linkage
# /subsystem:native - build for native (windows) system
# /driver:wdm - windows driver module specification
# -entry:DriverEntry - the entry of out driver (consider this as driver's <main>)
```

Starting a driver (explained):
```bash
sc create <your-desired-name> binPath= C:\...\driver.sys type= kernel # creating a service (driver) with the specific name in kernel mode
sc start <your-desired-name> # starting the service (driver)
```