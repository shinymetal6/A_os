/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usb_device.c
  * @version        : v1.0_Cube
  * @brief          : This file implements the USB Device
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/


#include "main.h"

#include "../../../../kernel/A.h"
#include "../../../../kernel/A_exported_functions.h"
#ifdef	STM32U575xx

#ifdef	USB_DEVICE_ENABLED
#include "../../usb_device_driver_manager.h"

#include "usb_device.h"
#include "../Core/usbd_core.h"

#include "usbd_CDC_desc.h"
#include "../Class/CDC/usbd_cdc.h"
#include "usbd_cdc_if.h"

USBD_HandleTypeDef hUsbDeviceHS;

uint8_t MX_Aos_USB_Device_Init(uint8_t usb_classdev)
{
	/* Init Device Library, add supported class and start the library. */
	if (( Asys.system_flags & SYS_FLAGS_USB_INITIALIZED) == SYS_FLAGS_USB_INITIALIZED )
		USBD_DeInit(&hUsbDeviceHS);
	switch ( usb_classdev )
	{
	case USB_CDC_CLASS:
		if (USBD_Init(&hUsbDeviceHS, &HS_CDC_Desc, DEVICE_FS) != USBD_OK)
			return 1;
		if (USBD_RegisterClass(&hUsbDeviceHS, &USBD_CDC) != USBD_OK)
			return 1;
		if (USBD_CDC_RegisterInterface(&hUsbDeviceHS, &USBD_CDC_Interface_fops_HS) != USBD_OK)
			return 1;
		if (USBD_Start(&hUsbDeviceHS) != USBD_OK)
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

#endif // #ifdef USB_DEVICE_ENABLED
#endif // #ifdef	STM32U575xx
