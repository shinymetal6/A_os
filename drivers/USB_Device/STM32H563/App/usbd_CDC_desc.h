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
 * usbd_CDC_desc.h
 *
 *  Created on: Oct 3, 2025
 *      Author: fil
 */

#ifndef DRIVERS_USB_DEVICE_STM32H563_APP_USBD_CDC_DESC_H_
#define DRIVERS_USB_DEVICE_STM32H563_APP_USBD_CDC_DESC_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#include "../../../../kernel/system_default.h"
#ifdef STM32H563xx
#ifdef USB_DEVICE_ENABLED
#include "../../../../kernel/A_exported_functions.h"

 #include "../Core/usbd_def.h"

/* USER CODE BEGIN INCLUDE */

/* USER CODE END INCLUDE */


/** @defgroup USBD_DESC USBD_DESC
  * @brief Usb device descriptors module.
  * @{
  */

/** @defgroup USBD_DESC_Exported_Constants USBD_DESC_Exported_Constants
  * @brief Constants.
  * @{
  */
#define         DEVICE_ID1          (UID_BASE)
#define         DEVICE_ID2          (UID_BASE + 0x4)
#define         DEVICE_ID3          (UID_BASE + 0x8)

#define         USB_SIZ_STRING_SERIAL       0x1A

/* USER CODE BEGIN EXPORTED_CONSTANTS */

/* USER CODE END EXPORTED_CONSTANTS */

/**
  * @}
  */

/** @defgroup USBD_DESC_Exported_Defines USBD_DESC_Exported_Defines
  * @brief Defines.
  * @{
  */

/* USER CODE BEGIN EXPORTED_DEFINES */

/* USER CODE END EXPORTED_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_DESC_Exported_TypesDefinitions USBD_DESC_Exported_TypesDefinitions
  * @brief Types.
  * @{
  */

/* USER CODE BEGIN EXPORTED_TYPES */

/* USER CODE END EXPORTED_TYPES */

/**
  * @}
  */

/** @defgroup USBD_DESC_Exported_Macros USBD_DESC_Exported_Macros
  * @brief Aliases.
  * @{
  */

/* USER CODE BEGIN EXPORTED_MACRO */

/* USER CODE END EXPORTED_MACRO */

/**
  * @}
  */

/** @defgroup USBD_DESC_Exported_Variables USBD_DESC_Exported_Variables
  * @brief Public variables.
  * @{
  */

extern USBD_DescriptorsTypeDef     CDC_Desc;

#endif // #ifdef STM32H563xx
#endif // #ifdef USB_DEVICE_ENABLED
#ifdef __cplusplus
}
#endif


#endif /* DRIVERS_USB_DEVICE_STM32H563_APP_USBD_CDC_DESC_H_ */
