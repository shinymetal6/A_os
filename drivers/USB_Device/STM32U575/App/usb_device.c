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

#ifdef	STM32U575xx
#include "../../../../kernel/system_default.h"
#ifdef USB_DEVICE_ENABLED

#include "usb_device.h"
#include "../Core/usbd_core.h"
#include "usbd_desc.h"
#include "../Class/CDC/usbd_cdc.h"
#include "usbd_cdc_if.h"

USBD_HandleTypeDef hUsbDeviceHS;

uint8_t MX_USB_Device_Init(void)
{
  if (USBD_Init(&hUsbDeviceHS, &HS_Desc, DEVICE_HS) != USBD_OK)
	  return 1;

  if (USBD_RegisterClass(&hUsbDeviceHS, &USBD_CDC) != USBD_OK)
	  return 1;

  if (USBD_CDC_RegisterInterface(&hUsbDeviceHS, &USBD_Interface_fops_HS) != USBD_OK)
	  return 1;

  if (USBD_Start(&hUsbDeviceHS) != USBD_OK)
	  return 1;
  return 0;
}
#endif // #ifdef USB_DEVICE_ENABLED
#endif // #ifdef	STM32U575xx
