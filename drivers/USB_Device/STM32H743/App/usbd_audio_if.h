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
 * usbd_audio_if.h
 *
 *  Created on: May 21, 2025
 *      Author: fil
 */

#ifndef DRIVERS_USB_DEVICE_STM32H743_APP_USBD_AUDIO_IF_H_
#define DRIVERS_USB_DEVICE_STM32H743_APP_USBD_AUDIO_IF_H_

#ifdef	STM32H743xx

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#include "../../../../kernel/system_default.h"
#ifdef	USB_AUDIO

/* Includes ------------------------------------------------------------------*/
#include "../Class/AUDIO/usbd_audio.h"

extern USBD_AUDIO_ItfTypeDef USBD_Interface_fops_FS;
extern USBD_HandleTypeDef hUsbDeviceFS;

void TransferComplete_CallBack_FS(void);

void HalfTransfer_CallBack_FS(void);

#endif //ifdef	USB_AUDIO

#endif //#ifdef	STM32H743xx
#endif /* DRIVERS_USB_DEVICE_STM32H743_APP_USBD_AUDIO_IF_H_ */
