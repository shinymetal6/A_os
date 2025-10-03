/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_cdc_if.h
  * @version        : v1.0_Cube
  * @brief          : Header for usbd_cdc_if.c file.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CDC_IF_H__
#define __USBD_CDC_IF_H__

#ifdef	STM32U575xx
#include "../../../../kernel/system_default.h"
#ifdef USB_DEVICE_ENABLED
#include "../Class/CDC/usbd_cdc.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define APP_RX_DATA_SIZE  2048
#define APP_TX_DATA_SIZE  2048


extern USBD_CDC_ItfTypeDef USBD_CDC_Interface_fops_HS;


uint8_t CDC_Transmit_HS(uint8_t* Buf, uint16_t Len);

#ifdef __cplusplus
}
#endif
#endif // #ifdef USB_DEVICE_ENABLED
#endif // #ifdef	STM32U575xx
#endif /* __USBD_CDC_IF_H__ */

