# A_os
v2025.10.00-rc0<br>
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
# New settings for user processes<br>
1 : added user include file for "project_lib_modules.h", so the settings can be defined in user space.<br>
2 : the file "project_lib_modules.h" in user processes now is mandatory. <br>See template in boards directory.<br>
3 : added 	void (*user_init)(uint32_t); in USRprcs_t called before starting a process.<br>
Please note : there is no real need to use the early initialization compiling the user_init in USRprcs_t<br>
excluding some long call init functions like lcds or some I2C or SPI drivers.<br>
<br>
# New USB functionality<br>
USB can be initialized as usual.<br>
If USB has a normal use there is no need to add the callback in the declaration.<br>
Where the user needs a very quick response from the USB layer ( e.g. MIDI )the callback can point directly<br>
to the user function that manages MIDI messages.<br>



