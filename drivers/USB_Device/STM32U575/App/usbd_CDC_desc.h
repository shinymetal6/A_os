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

#ifndef DRIVERS_USB_DEVICE_STM32U575_APP_USBD_CDC_DESC_H_
#define DRIVERS_USB_DEVICE_STM32U575_APP_USBD_CDC_DESC_H_

#ifdef __cplusplus
 extern "C" {
#endif

#ifdef	STM32U575xx
#ifdef USB_DEVICE_ENABLED

 #include "../Core/usbd_def.h"

#define         DEVICE_ID1          (UID_BASE)
#define         DEVICE_ID2          (UID_BASE + 0x4)
#define         DEVICE_ID3          (UID_BASE + 0x8)

#define  USB_SIZ_STRING_SERIAL       0x1A

extern USBD_DescriptorsTypeDef HS_CDC_Desc;

#endif // #ifdef USB_DEVICE_ENABLED
#endif // #ifdef	STM32U575xx

#endif /* DRIVERS_USB_DEVICE_STM32U575_APP_USBD_CDC_DESC_H_ */
