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
 * usb_device_driver_manager.h
 *
 *  Created on: Nov 7, 2024
 *      Author: fil
 */
#ifndef DRIVERS_USB_DRIVER_MANAGER_H_
#define DRIVERS_USB_DRIVER_MANAGER_H_

#define		USB_DRIVER_RX_SIZE		64
#define		USB_DRIVER_TX_SIZE		64

typedef struct
{
	uint8_t				status;
	uint8_t				flags;
	uint16_t			rx_num_chars;
	uint16_t			requested_len;
	uint16_t			data_index;
	uint8_t				*data;
	uint16_t			timeout;
	uint16_t			timeout_reload_value;
	void				(*Rx_CallbackPtr)(uint8_t* buf, uint16_t len);
	uint32_t 			wakeup_id;
}USB_Drv_TypeDef;

typedef struct
{
	uint8_t 	process;
	uint8_t		status;
	uint8_t		flags;
	USB_Drv_TypeDef		*usb_driver_private_data;
}USB_DriverStruct_t;

/*
extern	uint8_t MX_USB_DEVICE_Init(void);
extern	uint8_t MX_USB_Device_Init(void);
*/

extern	uint32_t	usb_device_driver_register(USB_Drv_TypeDef *usb_driver_private_data);
extern	uint32_t 	usb_device_driver_set_rx_buffer(uint8_t handle,uint8_t *rx_buf);
extern 	uint16_t 	usb_get_rx_len(uint8_t handle);
extern	uint32_t 	usb_send(uint8_t handle,uint8_t* ptr, uint16_t len);

#endif /* DRIVERS_USB_DRIVER_MANAGER_H_ */
