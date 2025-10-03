/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           USB_Device/CDC_Standalone/USB_Device/App/usb_device.h
  * @author         MCD Application Team
  * @brief          Header for usb_device.c file.
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
#ifndef __USB_DEVICE__H__
#define __USB_DEVICE__H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#include "../../../../kernel/system_default.h"
#ifdef STM32H563xx
#ifdef USB_DEVICE_ENABLED
#include "../../../../kernel/A_exported_functions.h"

 #include "stm32h5xx.h"
#include "stm32h5xx_hal.h"
#include "../Core/usbd_def.h"

#endif // #ifdef STM32H563xx
#endif // #ifdef USB_DEVICE_ENABLED

#ifdef __cplusplus
}
#endif

#endif /* __USB_DEVICE__H__ */
