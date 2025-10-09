/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    USB_Device/CDC_Standalone/USB_Device/App/usb_device.c
  * @author  MCD Application Team
  * @brief   This file implements the USB Device
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"
#include "../../../../kernel/A.h"
#include "../../../../kernel/A_exported_functions.h"
#ifdef STM32H563xx
#ifdef USB_DEVICE_ENABLED

#include "usb_device.h"
#include "../Core/usbd_core.h"
#include "usbd_CDC_desc.h"
#include "../Class/CDC/usbd_cdc.h"
#include "usbd_cdc_if.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
__IO uint32_t remotewakeupon = 0;
uint8_t HID_Buffer[4];
extern PCD_HandleTypeDef hpcd_USB_FS;

/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */


/* USB Device Core handle declaration. */
USBD_HandleTypeDef hUsbDeviceFS;
extern USBD_DescriptorsTypeDef CDC_Desc;

/*
 * -- Insert your variables declaration here --
 */

/* USER CODE END 0 */

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/*
void MX_USB_Device_Init(void)
{
  if (USBD_Init(&hUsbDeviceFS, &CDC_Desc, 0) != USBD_OK)
  {
    Error_Handler();
  }
  if (USBD_RegisterClass(&hUsbDeviceFS, &USBD_CDC) != USBD_OK)
  {
    Error_Handler();
  }
  if (USBD_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS) != USBD_OK)
  {
    Error_Handler();
  }
}
*/

uint8_t MX_Aos_USB_Device_Init(uint8_t usb_classdev)
{
	/* Init Device Library, add supported class and start the library. */
	if (( Asys.system_flags & SYS_FLAGS_USB_INITIALIZED) == SYS_FLAGS_USB_INITIALIZED )
		USBD_DeInit(&hUsbDeviceFS);
	switch ( usb_classdev )
	{
	case USB_CDC_CLASS:
		if (USBD_Init(&hUsbDeviceFS, &CDC_Desc, 0) != USBD_OK)
			return 1;
		if (USBD_RegisterClass(&hUsbDeviceFS, &USBD_CDC) != USBD_OK)
			return 1;
		if (USBD_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS) != USBD_OK)
			return 1;
		HAL_Delay(1000);
		if (USBD_Start(&hUsbDeviceFS) != USBD_OK)
			return 1;
		HAL_Delay(1000);
		break;
		/*
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
		*/
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
#endif // #ifdef STM32H563xx
#endif // #ifdef USB_DEVICE_ENABLED
