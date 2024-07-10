/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbh_platform.c

  * @brief          : This file implements the USB platform
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
#include "main.h"

#ifdef	STM32F746xx
#include "../../../../../kernel/system_default.h"

#ifdef	USB_HOST
#include "usbh_platform.h"

void MX_DriverVbusHS(uint8_t state)
{
  uint8_t data = state;
  /* USER CODE BEGIN PREPARE_GPIO_DATA_VBUS_HS */
  if(state == 0)
  {
    /* Drive high Charge pump */
    data = GPIO_PIN_SET;
  }
  else
  {
    /* Drive low Charge pump */
    data = GPIO_PIN_RESET;
  }
  /* USER CODE END PREPARE_GPIO_DATA_VBUS_HS */
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_5,(GPIO_PinState)data);
}


#endif // #ifdef	USB_HOST

#endif // #ifdef	STM32F746xx
