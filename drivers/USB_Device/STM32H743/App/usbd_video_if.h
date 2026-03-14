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
 * usbd_video_if.h
 *
 *  Created on: Mar 2, 2026
 *      Author: fil
 */

#ifndef DRIVERS_USB_DEVICE_STM32H743_APP_USBD_VIDEO_IF_H_
#define DRIVERS_USB_DEVICE_STM32H743_APP_USBD_VIDEO_IF_H_


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "../Class/VIDEO/usbd_video.h"

extern USBD_VIDEO_ItfTypeDef USBD_VIDEO_fops_FS;

#define JPEG_SIZE 1
#define UVC_ISO_DATA_PACKET_SIZE 150

void TransferComplete_CallBack_FS(void);

/**
  * @brief  Manages the DMA half transfer complete event.
  * @retval None
  */
void HalfTransfer_CallBack_FS(void);

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_USB_DEVICE_STM32H743_APP_USBD_VIDEO_IF_H_ */
