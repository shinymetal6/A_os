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
 * hwmanager.c
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */

#include "main.h"
#include "A.h"
#include "scheduler.h"
#include "A_exported_functions.h"
//#include "kernel_opt.h"

extern	HWMngr_t		HWMngr[PERIPHERAL_NUM];
extern	HWDevices_t		HWDevices[HWDEVICES_NUM];

extern	Asys_t			Asys;
uint8_t	stored_peripheral;

ITCM_AREA_CODE uint32_t allocate_hw(uint8_t peripheral,uint8_t config)
{
	__disable_irq();
	stored_peripheral = peripheral;
	if ( peripheral >= PERIPHERAL_NUM)
		return 0;
	if ( HWMngr[peripheral].process )
		return 0;
	HWMngr[peripheral].process = Asys.current_process;
	HWMngr[peripheral].status = HWMAN_STATUS_ALLOCATED | config;
	HWMngr[peripheral].irq_callback = NULL;
	__enable_irq();
	return peripheral;
}

/* bus_peripheral : the bus where the device is connected to. i.e. HW_SPI2 */
/* device_peripheral : the device, i.e HW_NRF24L01 */
ITCM_AREA_CODE uint32_t allocate_hw_with_irq_callback(uint8_t bus_peripheral,uint8_t device_peripheral,uint8_t dev_config,void (*irq_callback)(void))
{
	__disable_irq();
	if (( HWMngr[bus_peripheral].process ) || ( HWMngr[device_peripheral].process ))
		return 0;
	HWMngr[bus_peripheral].process = Asys.current_process;
	HWMngr[bus_peripheral].status  = HWMAN_STATUS_ALLOCATED;
	HWMngr[bus_peripheral].irq_callback = irq_callback;
	HWDevices[device_peripheral].process = Asys.current_process;
	HWDevices[device_peripheral].status = HWDEV_STATUS_ALLOCATED | dev_config;
	HWDevices[device_peripheral].device_peripheral = device_peripheral;
	HWDevices[device_peripheral].irq_callback = irq_callback;
	HWDevices[device_peripheral].bus_peripheral = bus_peripheral;
	__enable_irq();
	return bus_peripheral;
}

ITCM_AREA_CODE uint8_t get_busdevice_from_device(uint8_t device_peripheral)
{
uint32_t	i;
	if ( HWDevices[device_peripheral].process == Asys.current_process)
	{
		for(i=0;i<HWDEVICES_NUM;i++)
			if ( HWDevices[i].device_peripheral == device_peripheral)
				return HWDevices[i].bus_peripheral;
	}
	return 0;
}

ITCM_AREA_CODE uint32_t deallocate_hw(uint8_t peripheral)
{
	__disable_irq();
	if ( HWMngr[peripheral].process != Asys.current_process )
		return 0;
	HWMngr[peripheral].process = 0;
	HWMngr[peripheral].status = HWMAN_STATUS_FREE;
	HWMngr[peripheral].irq_callback = NULL;
	__enable_irq();
	return peripheral;
}

ITCM_AREA_CODE uint16_t get_rx_len(uint8_t peripheral)
{
	if ( HWMngr[peripheral].process != Asys.current_process )
		return 0;
	return HWMngr[HW_USB_DEVICE].rxlen;
}

ITCM_AREA_CODE uint32_t get_peripheral_flags(uint8_t peripheral)
{
	if ( HWMngr[peripheral].process != Asys.current_process )
		return 0;
	return HWMngr[HW_USB_DEVICE].flags;
}

