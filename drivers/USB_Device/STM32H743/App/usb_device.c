/* 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Project : A_os
*/
/*
 * usb_device.c
 *
 *  Created on: Jan 23, 2024
 *      Author: fil
 */

#include "main.h"
#ifdef	STM32H743xx
#include "../../../../kernel/system_default.h"

#ifdef	USB_DEVICE_ENABLED
#include "../../../../kernel/A_exported_functions.h"
#include "../../usb_device_driver_manager.h"

#include "usb_device.h"
#include "../Core/usbd_core.h"

#include "usbd_CDC_desc.h"
#include "../Class/CDC/usbd_cdc.h"
#include "usbd_cdc_if.h"

#include "usbd_MIDI_desc.h"
#include "../Class/MIDI/usbd_midi.h"
#include "usbd_midi_if.h"

/* USB Device Core handle declaration. */
USBD_HandleTypeDef hUsbDeviceFS;

extern	USBD_DescriptorsTypeDef FS_CDC_Desc;
extern	USBD_CDC_ItfTypeDef USBD_CDC_Interface_fops_FS;

extern	USBD_DescriptorsTypeDef FS_MIDI_Desc;
extern	USBD_MIDI_ItfTypeDef USBD_MIDI_Interface_fops_FS;

uint8_t MX_Aos_USB_Device_Init(uint8_t usb_classdev)
{
	/* Init Device Library, add supported class and start the library. */
	if (( Asys.system_flags & SYS_FLAGS_USB_INITIALIZED) == SYS_FLAGS_USB_INITIALIZED )
		USBD_DeInit(&hUsbDeviceFS);
	switch ( usb_classdev )
	{
	case USB_CDC_CLASS:
		if (USBD_Init(&hUsbDeviceFS, &FS_CDC_Desc, DEVICE_FS) != USBD_OK)
			return 1;
		if (USBD_RegisterClass(&hUsbDeviceFS, &USBD_CDC) != USBD_OK)
			return 1;
		if (USBD_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_CDC_Interface_fops_FS) != USBD_OK)
			return 1;
		if (USBD_Start(&hUsbDeviceFS) != USBD_OK)
			return 1;
		break;
	case USB_MIDI_CLASS:
		if (USBD_Init(&hUsbDeviceFS, &FS_MIDI_Desc, DEVICE_FS) != USBD_OK)
			return 1;
		if (USBD_RegisterClass(&hUsbDeviceFS, &USBD_MIDI) != USBD_OK)
			return 1;
		if (USBD_MIDI_RegisterInterface(&hUsbDeviceFS, &USBD_MIDI_Interface_fops_FS) != USBD_OK)
			return 1;
		if (USBD_Start(&hUsbDeviceFS) != USBD_OK)
			return 1;
		break;
	}
	Asys.system_flags |= SYS_FLAGS_USB_INITIALIZED;
	return 0;
}

uint8_t MX_USB_Device_Init(void)
{
#ifdef OLD_USB
	/* Init Device Library, add supported class and start the library. */
#ifdef	USB_CDC
	MX_Aos_USB_Device_Init(0);
#endif
#ifdef	USB_MIDI
	MX_Aos_USB_Device_Init(1);
#endif
#ifdef	USB_AUDIO
	MX_Aos_USB_Device_Init(2);
#endif
#endif
	return 0;
}

#endif // #ifdef	USB_ENABLED

#endif // #ifdef	STM32H743xx
