/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbh_platform.h
  * @brief          : Header for usbh_platform.c file.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBH_PLATFORM_H__
#define __USBH_PLATFORM_H__
#include "main.h"

#ifdef	STM32F746xx
#include "../../../../../kernel/system_default.h"

#ifdef	USB_HOST

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "../App/usb_host.h"

/* USER CODE BEGIN INCLUDE */

/* USER CODE END INCLUDE */

void MX_DriverVbusHS(uint8_t state);

#ifdef __cplusplus
}
#endif

#endif // #ifdef	USB_HOST

#endif // #ifdef	STM32F746xx
#endif /* __USBH_PLATFORM_H__ */

