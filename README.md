# A_os
v2025.11.00-rc2<br>
!!! NOTE !!!<br>
Still this runs with <br>
STM32CubeIDE Version: 1.18.1 Build: 24813_20250409_2138 (UTC)<br>
The developing machine is :<br>
OS: Linux, v.5.15.0-139-generic, x86_64 / gtk 3.24.20<br>
Java vendor: Eclipse Adoptium<br>
Java runtime version: 21.0.3+9-LTS<br>
Java version: 21.0.3<br>
<br><br>
Latest 19.x has some severe bugs both on IDE and libraries.<br>
DON'T USE 19.x<br>
<br><br>
# New tree<br>
From v2025.10.00 onward, the directory "board" has been replaced with the directory "processors" with a different logic<br>
The "processors" directory contains one include file for each supported processor.<br>
# New settings for user processes<br>
1 : added user include file for "user_config.h", so the settings can be defined in user space.<br>
2 : the file "user_config.h" in user processes now is mandatory. See the template in boards directory.<br>
3 : added 	void (*user_init)(uint32_t); in USRprcs_t called before starting a process.<br>
4 : the user processes directory now MUST be included in the include compilator directive.<br>
Please note : there is no real need to use the early initialization compiling the user_init in USRprcs_t<br>
excluding some long call init functions like lcds or some I2C or SPI drivers.<br>
<br>
# New USB functionality<br>
USB can be initialized as usual.<br>
If USB has a normal use there is no need to add the callback in the declaration.<br>
Where the user needs a very quick response from the USB layer ( e.g. MIDI ) the callback can point directly<br>
to the user function that manages MIDI messages.<br>
This means the user code runs actually under interrupt routine, so should be quick enough to leave space to other processes.<br>
# New Sample processes logic<br>
If there is no "user_config.h" file the system defaults to sample processes.<br>
This leaves A_os untouched when switching from Sample Processes to user processes.<br>
Just having a "user_config.h" in the user processes area disables the Sample Processes<br>
It's still under user responsibility to have a conveniently compiled "user_config.h".<br>
Please note : the path of the user processes tree MUST be explicitly declared in the "Include File" tab in Compiler Settings
# Added QtConfigurator<br>
QtConfigurator is a simple tool to generate user_config.h file. Still in wip.<br>
QtConfigurator is in Qt directory.<br>

# Quick start<br>
1) Copy processor ld file from LdFiles folder in kernel overwiriting the .ld file from Cube<br>
2) Disable compilation of the USB Cube generated files if USB is used<br>
3) Disable the appropriate entries in stm32h7xx_it.c<br>
4) Add void A_init_mem(void); and void A_start(void); in main.h<br>
5) Add the call to A_init_mem(); in main.c just before peripheral initialization<br>
6) Add the call to A_start(); just after the peripherals initializations.<br>
7) Enable the appropriate sample in sample_processes_includes.h

# Processors / Boards currently supported : <br>
STM32H743xx : Nucleo and other hardware<br>
STM32H753xx : Nucleo and other hardware<br>
STM32F722xx : Nucleo<br>
STM32G474xx : Nucleo and other hardware<br>
STM32U575xx : Nucleo and other hardware ( still DMA on uart RX not running )<br>
STM32F446xx : Nucleo<br>
STM32L152xE : Nucleo and other hardware<br>






