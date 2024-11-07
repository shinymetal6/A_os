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
 * extmem_driver_manager.c
 *
 *  Created on: Nov 5, 2024
 *      Author: fil
 */

#include "main.h"
#include "../kernel/system_default.h"
#include "../kernel/A.h"
#include "../kernel/A_exported_functions.h"
#include "../kernel/scheduler.h"
#include "../kernel/kernel_opt.h"
#include <string.h>
#include "extmem_driver_manager.h"

extern		MemDriverStruct_t	*MemDriverStruct[MAX_MEM_DRIVERS];

SYSTEM_RAM	uint8_t			extmem_last_used_handle=0,extmem_driver_request = 0;

ITCM_AREA_CODE uint32_t	extmem_driver_register(MemDriverStruct_t *driver,uint32_t *private_drv_struct,uint32_t flags)
{
	if ( MemDriverStruct[extmem_last_used_handle] == NULL )
	{
		MemDriverStruct[extmem_last_used_handle] = driver;
		MemDriverStruct[extmem_last_used_handle]->process = get_current_process();
		MemDriverStruct[extmem_last_used_handle]->flags |= flags;
		MemDriverStruct[extmem_last_used_handle]->driver_private_data = private_drv_struct;

		if (driver->periodic_before_check_timers_callback != NULL  )
			set_before_check_timers_callback(driver->periodic_before_check_timers_callback);
		if (driver->periodic_after_check_timers_callback != NULL  )
			set_after_check_timers_callback(driver->periodic_after_check_timers_callback);
		MemDriverStruct[extmem_last_used_handle]->status = DRIVER_STATUS_REQUESTED;

		extmem_last_used_handle++;
		extmem_driver_request++;
		return extmem_last_used_handle-1;
	}
	return DRIVER_REQUEST_FAILED;
}

ITCM_AREA_CODE uint32_t	extmem_driver_unregister(const MemDriverStruct_t *driver)
{
uint32_t	i;
	for(i=0;i<MAX_MEM_DRIVERS;i++)
	{
		if ( strcmp(MemDriverStruct[i]->driver_name,driver->driver_name) == 0 )
		{
			MemDriverStruct[i] = NULL;
			extmem_last_used_handle--;
			return 0;
		}
	}
	return 1;
}

ITCM_AREA_CODE uint32_t extmem_driver_scan(void)
{
uint32_t	i;
	if (extmem_driver_request )
	{
		for(i=0;i<MAX_MEM_DRIVERS;i++)
		{
			if ( MemDriverStruct[i] == NULL )
				return DRIVER_STATUS_INITPEND;
			if (( MemDriverStruct[i]->status & DRIVER_STATUS_REQUESTED) ==  DRIVER_STATUS_REQUESTED)
			{
				MemDriverStruct[i]->status = DRIVER_STATUS_IN_USE;
				extmem_driver_request --;
				if ( extmem_driver_request == 0)
					return DRIVER_STATUS_UNUSED;
			}
		}
	}
	return DRIVER_STATUS_INITIALIZED;
}

ITCM_AREA_CODE uint32_t extmem_driver_read(uint32_t handle,uint32_t address,uint8_t *data,uint16_t data_len)
{
	if ( handle > MAX_MEM_DRIVERS )
		return DRIVER_REQUEST_FAILED;
	if ( MemDriverStruct[handle]->read != NULL )
		return MemDriverStruct[handle]->read(handle,address,data,data_len);
	return DRIVER_REQUEST_FAILED;
}

ITCM_AREA_CODE uint32_t extmem_driver_read_config(uint32_t handle,uint32_t address,uint8_t *data,uint16_t data_len)
{
	if ( handle > MAX_MEM_DRIVERS )
		return DRIVER_REQUEST_FAILED;
	if ( MemDriverStruct[handle]->read_config != NULL )
		return MemDriverStruct[handle]->read_config(handle,address,data,data_len);
	return DRIVER_REQUEST_FAILED;
}

ITCM_AREA_CODE uint32_t extmem_driver_write(uint32_t handle,uint32_t address,uint8_t *data,uint16_t data_len)
{
	if ( handle > MAX_MEM_DRIVERS )
		return DRIVER_REQUEST_FAILED;
	if ( MemDriverStruct[handle]->write != NULL )
		return MemDriverStruct[handle]->write(handle,address,data,data_len);
	return DRIVER_REQUEST_FAILED;
}

ITCM_AREA_CODE uint32_t extmem_get_status(uint32_t handle)
{
	if ( handle > MAX_MEM_DRIVERS )
		return DRIVER_REQUEST_FAILED;
	if ( MemDriverStruct[handle]->get_status != NULL )
		return MemDriverStruct[handle]->get_status(handle);
	return DRIVER_REQUEST_FAILED;
}

ITCM_AREA_CODE uint32_t 	 extmem_driver_init(void)
{
	return 0;
}


