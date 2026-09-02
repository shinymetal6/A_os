# A_os
v2026.09-rc0<br>
Runs with <br>
STM32CubeIDE Version: STM32CubeIDE Version: 2.2.0 Build: 29186_20260626_0934 (UTC)<br>
OS: Linux, v.6.8.0-138-generic, x86_64 / gtk 3.24.33
Java vendor: Eclipse Adoptium
Java runtime version: 21.0.10+7-LTS
Java version: 21.0.10
<br>
The developing machine is :<br>
OS: Linux, v.5.15.0-139-generic, x86_64 / gtk 3.24.20<br>
Java vendor: Eclipse Adoptium<br>
Java runtime version: 21.0.3+9-LTS<br>
Java version: 21.0.3<br>
<br><br>
Latest 19.x has some severe bugs both on IDE and libraries.<br>
DON'T USE 19.x<br>
<br>
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
This means the user code callback function runs actually under interrupt routine, so should be quick enough to leave space to other processes.<br>
Please note : when st middleware is not used enable the "USB on the go FS global interrupt" in NVIC section 
# New Sample processes logic<br>
If there is no "user_config.h" file the system defaults to sample processes.<br>
This leaves A_os untouched when switching from Sample Processes to user processes.<br>
Just having a "user_config.h" in the user processes area disables the Sample Processes<br>
It's still under user responsibility to have a conveniently compiled "user_config.h".<br>
Please note : the path of the user processes tree MUST be explicitly declared in the "Include File" tab in Compiler Settings
# Added sd card driver in drivers and fat fs in modules<br>
Remember to disable the generated FATFS and Middlewares/Third_Party in compilation<br>
# Added QtConfigurator<br>
QtConfigurator is a simple tool to generate user_config.h file. Still in wip.<br>
QtConfigurator is in Qt directory.<br>
# New drivers functionality<br>
Initialization is now done with linked list in user struct<br>
# New processor support<br>
Initial support for STM32N657xx<br>


# Quick start<br>
1) Copy processor ld file from LdFiles folder in kernel overwiriting the .ld file from Cube<br>
2) Disable compilation of the USB Cube generated files if USB is used<br>
3) Disable the appropriate entries in user stm32pNxx_it.c. Use stm32pNxx_it.c.Example in SampleProcesses as example<br>
4) Add void A_init_mem(void); and void A_start(void); in main.h<br>
5) Add the call to A_init_mem(); in main.c just before peripheral initialization<br>
6) Add the call to A_start(); just after the peripherals initializations.<br>
7) Enable the appropriate sample in sample_processes_includes.h

# Processors / Boards currently supported : <br>
STM32H743xx : Nucleo and other hardware<br>
STM32H753xx : Nucleo and other hardware<br>
STM32F722xx : Nucleo<br>
STM32G474xx : Nucleo and other hardware<br>
STM32U575xx : Nucleo and other hardware<br>
STM32F446xx : Nucleo<br>
STM32L152xE : Nucleo and other hardware<br>
STM32H563xx : Nucleo<br>
STM32G431RB : Nucleo<br>
STM32G491RE : Nucleo<br>
STM32N657xx : Nucleo : NOTE: Still in wip<br>
<br>
Please note:<br>
The iodef_common.h file is not yet complete, so some definitions maybe not correct<br>

# SD Card notes<br>
As Cube IDE initializes the sd card well before calling Aos, an error is raised when<br>
 an empty slot is found.<br>
To avoid to hang the system some code must be modified before the Cube IDE initialization.<br>
This snippet is already present, and the procedure is:<br>
1) Locate the function static void MX_SDMMC1_SD_Init(void)<br>
2) Locate the line that tells : "if (HAL_SD_Init(&hsd1) != HAL_OK)"<br>
3) Remove the if condition, the line must be "HAL_SD_Init(&hsd1);"<br>

# Samples : <br>
SampleProcesses/sample_process_1_adc.c<br>
SampleProcesses/sample_process_1_audio_DAC_midi.c<br>
SampleProcesses/sample_process_1_audio_DAC_oscillator.c<br>
SampleProcesses/sample_process_1_audio_I2S_dual_oscillator.c<br>
SampleProcesses/sample_process_1_audio_I2S_in_effect_out.c<br>
SampleProcesses/sample_process_1_audio_I2S_midi.c<br>
SampleProcesses/sample_process_1_audio_I2S_oscillator.c<br>
SampleProcesses/sample_process_1_basic.c<br>
SampleProcesses/sample_process_1_can.c<br>
SampleProcesses/sample_process_1_dccpwm.c<br>
SampleProcesses/sample_process_1_Dhtxx_am230x.c<br>
SampleProcesses/sample_process_1_encoder.c<br>
SampleProcesses/sample_process_1_g431rb_pn5180.c<br>
SampleProcesses/sample_process_1_gpio.c<br>
SampleProcesses/sample_process_1_hc05.c<br>
SampleProcesses/sample_process_1_i2c.c<br>
SampleProcesses/sample_process_1_i2cmem.c<br>
SampleProcesses/sample_process_1_i2csensors.c<br>
SampleProcesses/sample_process_1_intflash.c<br>
SampleProcesses/sample_process_1_lcd7735.c<br>
SampleProcesses/sample_process_1_mbxToPrc2.c<br>
SampleProcesses/sample_process_1_midi.c<br>
SampleProcesses/sample_process_1_mlx90614.c<br>
SampleProcesses/sample_process_1_mlx90640.c<br>
SampleProcesses/sample_process_1_nrf24l01_ping.c<br>
SampleProcesses/sample_process_1_nrf24l01_pong.c<br>
SampleProcesses/sample_process_1_pid.c<br>
SampleProcesses/sample_process_1_pn5180.c<br>
SampleProcesses/sample_process_1_qspi.c<br>
SampleProcesses/sample_process_1_rx_DMA_UART.c<br>
SampleProcesses/sample_process_1_sdcard.c<br>
SampleProcesses/sample_process_1_servo.c<br>
SampleProcesses/sample_process_1_stepper.c<br>
SampleProcesses/sample_process_1_switch_midi_cdc.c<br>
SampleProcesses/sample_process_1_tof_vl53l5cx.c<br>
SampleProcesses/sample_process_1_usbclass_switch.c<br>
SampleProcesses/sample_process_1_usbecho.c<br>
SampleProcesses/sample_process_1_xmodem_rx_UART.c<br>
SampleProcesses/sample_process_1_xmodem_rx_USB.c<br>
SampleProcesses/sample_process_2_mbxFromPrc1.c<br>
SampleProcesses/sample_process_2.c<br>
SampleProcesses/sample_process_3.c<br>
SampleProcesses/sample_process_4.c<br>
SampleProcesses/sample_processes_table.c<br>
SampleProcesses/sample_support_functions.c<br>
SampleProcesses/sample_user_config.c<br>








