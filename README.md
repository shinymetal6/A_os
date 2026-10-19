# A_os
v2026.11-rc0<br>
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
# Major changes<br>
From v2025.11.00 onward, the structure is the same as ST, su USB,Lwip and FAT libraries come from<br>
Cube directly<br>
Also the sound folder vanished, will be replaced in next releases.<br>
# USB Adaptations<br>
In usb_cdc.c under USB_DEVICE/App in static int8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len) add the following<br>
	extern void usb_device_driver_pktreceived_callback(uint8_t* Buf, uint32_t Len);<br>
	usb_device_driver_pktreceived_callback(Buf,*Len);<br>
This calls the usb_device_driver_manager.c usb_device_driver_pktreceived_callback, and gives the data to the application<br>
# FATFS Adaptations<br>
Nothing to do here<br>
<br>
# WAV player<br>
Added the wav player under modules to leave the user free of all the bull**hit for the wav files<br>
Actually only 16bit PCM single channel is supported to DAC out<br>






