/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usb_device.c
  * @version        : v3.0_Cube
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
#include "../../../../kernel/A.h"
#include "../../../../kernel/A_exported_functions.h"
#ifdef A_OS_STM32G4xx_PROCESSOR
#ifdef USB_DEVICE_ENABLED
#include "usb_device.h"
#include "../Core/usbd_core.h"
#include "usbd_desc.h"
#include "../Class/CDC/usbd_cdc.h"
#include "usbd_cdc_if.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

extern void Error_Handler(void);
/* USB Device Core handle declaration. */
USBD_HandleTypeDef hUsbDeviceFS;
extern USBD_DescriptorsTypeDef CDC_Desc;

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * Init USB device Library, add supported class and start the library
  * @retval None
  */
uint8_t MX_Aos_USB_Device_Init(uint8_t usb_classdev)
{
	/* Init Device Library, add supported class and start the library. */
	if (( Asys.system_flags & SYS_FLAGS_USB_INITIALIZED) == SYS_FLAGS_USB_INITIALIZED )
		USBD_DeInit(&hUsbDeviceFS);
	  if (USBD_Init(&hUsbDeviceFS, &CDC_Desc, DEVICE_FS) != USBD_OK)
		  return 1;

	  if (USBD_RegisterClass(&hUsbDeviceFS, &USBD_CDC) != USBD_OK)
		  return 1;

	  if (USBD_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS) != USBD_OK)
		  return 1;

	  if (USBD_Start(&hUsbDeviceFS) != USBD_OK)
		  return 1;
	  return 0;
}

uint8_t MX_USB_Device_Init(void)
{
#ifdef OLDUSB
  /* USER CODE BEGIN USB_Device_Init_PreTreatment */

  /* USER CODE END USB_Device_Init_PreTreatment */

  /* Init Device Library, add supported class and start the library. */
  if (USBD_Init(&hUsbDeviceFS, &CDC_Desc, DEVICE_FS) != USBD_OK)
	  return 1;

  if (USBD_RegisterClass(&hUsbDeviceFS, &USBD_CDC) != USBD_OK)
	  return 1;

  if (USBD_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS) != USBD_OK)
	  return 1;

  if (USBD_Start(&hUsbDeviceFS) != USBD_OK)
	  return 1;

  /* USER CODE BEGIN USB_Device_Init_PostTreatment */
#endif
  return 0;
  /* USER CODE END USB_Device_Init_PostTreatment */
}

/**
  * @}
  */

/**
  * @}
  */

#endif // #ifdef STM32G4xx_USB
#endif // #ifdef	A_OS_STM32G4xx_PROCESSOR
