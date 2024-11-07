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
 * usb_driver_manager.h
 *
 *  Created on: Nov 7, 2024
 *      Author: fil
 */
#ifndef DRIVERS_USB_DRIVER_MANAGER_H_
#define DRIVERS_USB_DRIVER_MANAGER_H_

#define		USB_DRIVER_RX_SIZE		64

typedef struct
{
	uint8_t 	process;
	uint8_t		status;
	uint8_t		flags;
	uint8_t		handle;
	uint32_t	(*init)(uint8_t handle);
	uint32_t	(*deinit)(uint8_t handle);
	uint32_t	(*start)(uint8_t handle);
	uint32_t	(*stop)(uint8_t handle);
	uint32_t	(*get_status)(uint8_t handle);
	uint32_t	(*get_values)(uint8_t handle, uint8_t *values,uint16_t values_number);
	uint32_t	(*set_status)(uint8_t handle);
	uint32_t	(*set_values)(uint8_t handle, uint8_t *values,uint16_t values_number);
	uint32_t	(*extended_action)(uint32_t handle,uint32_t *action);
	void 		(*periodic_before_check_timers_callback)(void);
	void 		(*periodic_after_check_timers_callback)(void);
	uint8_t		*driver_private_data;
	uint8_t		*rx_buf;
	uint16_t	rx_buf_len;

}USB_DriverStruct_t;

extern	void MX_USB_DEVICE_Init(void);
extern	uint8_t MX_USB_Device_Init(void);


extern	uint32_t	usb_device_driver_register(USB_DriverStruct_t *driver);
extern	uint32_t 	usb_device_driver_scan(void);
extern	uint32_t 	usb_device_driver_set_rx_buffer(uint8_t *rx_buf);
extern 	uint16_t 	usb_device_driver_get_rx_len(void);
extern	uint32_t 	usb_device_driver_send(uint8_t* ptr, uint16_t len);

#endif /* DRIVERS_USB_DRIVER_MANAGER_H_ */
