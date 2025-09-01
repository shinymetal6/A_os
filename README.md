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
2 : added 	void (*user_init)(uint32_t); in USRprcs_t called before starting a process. <br>
<br>



