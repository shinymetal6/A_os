/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file            : usb_host.c
  * @version         : v1.0_Cube
  * @brief           : This file implements the USB Host
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

#include "usb_host.h"
#include "../../STM32_USB_Host_Library/Core/usbh_core.h"
#include "../../STM32_USB_Host_Library/Class/AUDIO/usbh_audio.h"
#include "../../STM32_USB_Host_Library/Class/CDC/usbh_cdc.h"
#include "../../STM32_USB_Host_Library/Class/MSC/usbh_msc.h"
#include "../../STM32_USB_Host_Library/Class/HID/usbh_hid.h"
#include "../../STM32_USB_Host_Library/Class/MTP/usbh_mtp.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USB Host core handle declaration */
USBH_HandleTypeDef hUsbHostHS;
ApplicationTypeDef Appli_state = APPLICATION_IDLE;

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*
 * user callback declaration
 */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * Init USB host library, add supported class and start the library
  * @retval None
  */
void MX_USB_HOST_Init(void)
{
  /* USER CODE BEGIN USB_HOST_Init_PreTreatment */

  /* USER CODE END USB_HOST_Init_PreTreatment */

  /* Init host Library, add supported class and start the library. */
  if (USBH_Init(&hUsbHostHS, USBH_UserProcess, HOST_HS) != USBH_OK)
  {
    Error_Handler();
  }
#ifdef	USB_AUDIO_HOST
  if (USBH_RegisterClass(&hUsbHostHS, USBH_AUDIO_CLASS) != USBH_OK)
  {
    Error_Handler();
  }
#endif // #ifdef	USB_AUDIO_HOST

#ifdef	USB_CDC_HOST
  if (USBH_RegisterClass(&hUsbHostHS, USBH_CDC_CLASS) != USBH_OK)
  {
    Error_Handler();
  }
#endif // #ifdef	USB_CDC_HOST

#ifdef	USB_MSC_HOST
  if (USBH_RegisterClass(&hUsbHostHS, USBH_MSC_CLASS) != USBH_OK)
  {
    Error_Handler();
  }
#endif // #ifdef	USB_MSC_HOST

#ifdef	USB_HID_HOST
  if (USBH_RegisterClass(&hUsbHostHS, USBH_HID_CLASS) != USBH_OK)
  {
    Error_Handler();
  }
#endif // #ifdef	USB_HID_HOST

#ifdef	USB_MTP_HOST
  if (USBH_RegisterClass(&hUsbHostHS, USBH_MTP_CLASS) != USBH_OK)
  {
    Error_Handler();
  }
#endif // #ifdef	USB_MTP_HOST

  if (USBH_Start(&hUsbHostHS) != USBH_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_HOST_Init_PostTreatment */

  /* USER CODE END USB_HOST_Init_PostTreatment */
}

/*
 * Background task
 */
void MX_USB_HOST_Process(void)
{
  /* USB Host Background task */
  USBH_Process(&hUsbHostHS);
}
/*
 * user callback definition
 */
static void USBH_UserProcess  (USBH_HandleTypeDef *phost, uint8_t id)
{
  /* USER CODE BEGIN CALL_BACK_1 */
  switch(id)
  {
  case HOST_USER_SELECT_CONFIGURATION:
  break;

  case HOST_USER_DISCONNECTION:
  Appli_state = APPLICATION_DISCONNECT;
  break;

  case HOST_USER_CLASS_ACTIVE:
  Appli_state = APPLICATION_READY;
  break;

  case HOST_USER_CONNECTION:
  Appli_state = APPLICATION_START;
  break;

  default:
  break;
  }
  /* USER CODE END CALL_BACK_1 */
}

#endif // #ifdef	USB_HOST

#endif // #ifdef	STM32F746xx
