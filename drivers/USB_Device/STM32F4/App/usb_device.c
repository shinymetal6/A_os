/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usb_device.c
  * @version        : v1.0_Cube
  * @brief          : This file implements the USB Device
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#ifdef	STM32F446xx
#include "../../../../kernel/A.h"
#include "../../../../kernel/A_exported_functions.h"
#ifdef	USB_DEVICE_ENABLED

#include "../../usb_device_driver_manager.h"

#include "usb_device.h"
#include "../Core/usbd_core.h"
#include "usbd_desc.h"
#include "../Class/CDC/usbd_cdc.h"
#include "usbd_cdc_if.h"

//extern USBD_HandleTypeDef hUsbDeviceFS;
USBD_HandleTypeDef hUsbDeviceFS;

uint8_t MX_Aos_USB_Device_Init(uint8_t usb_classdev)
{
	  if (USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS) != USBD_OK)
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
	  if (USBD_Start(&hUsbDeviceFS) != USBD_OK)
	  {
	    Error_Handler();
	  }
	  return 0;
}

uint8_t MX_USB_DEVICE_Init(void)
{
	MX_Aos_USB_Device_Init(USB_CDC_CLASS);

#ifdef OLD
  /* USER CODE END USB_DEVICE_Init_PreTreatment */

  /* Init Device Library, add supported class and start the library. */
  if (USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS) != USBD_OK)
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
  if (USBD_Start(&hUsbDeviceFS) != USBD_OK)
  {
    Error_Handler();
  }
#endif
  return 0;
}
#endif // #ifdef	USB_ENABLED

#endif // #ifdef	STM32F446xx

