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
 * extmem_driver_manager.h
 *
 *  Created on: Nov 5, 2024
 *      Author: fil
 */
#ifndef DRIVERS_EXTMEM_DRIVER_MANAGER_H_
#define DRIVERS_EXTMEM_DRIVER_MANAGER_H_

typedef struct
{
	uint8_t 	process;
	uint8_t		status;
	uint8_t		flags;
	uint8_t		handle;
	uint32_t	(*init)(uint8_t handle);
	uint32_t	(*deinit)(uint8_t handle);
	uint32_t	(*get_status)(uint8_t handle);
	uint32_t	(*read) (uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len);
	uint32_t	(*read_config) (uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len);
	uint32_t	(*write)(uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len);
	void 		(*periodic_before_check_timers_callback)(void);
	void 		(*periodic_after_check_timers_callback)(void);
	uint32_t	*driver_private_data;
	char		driver_name[32];
}MemDriverStruct_t;

extern	uint32_t	extmem_driver_register(MemDriverStruct_t *driver,uint32_t *private_drv_struct,uint32_t flags);
extern	uint32_t	extmem_driver_unregister(const MemDriverStruct_t *driver);
extern	uint32_t 	extmem_driver_scan(void);
extern	uint32_t 	extmem_driver_read(uint32_t handle,uint32_t address,uint8_t *data,uint16_t data_len);
extern	uint32_t	extmem_driver_read_config(uint32_t handle,uint32_t address,uint8_t *data,uint16_t data_len);
extern	uint32_t 	extmem_driver_write(uint32_t handle,uint32_t address,uint8_t *data,uint16_t data_len);
extern 	uint32_t 	extmem_get_status(uint32_t handle);
extern 	uint32_t 	extmem_driver_init(void);

#include "external_flash/i2c_24xx.h"

#endif /* DRIVERS_EXTMEM_DRIVER_MANAGER_H_ */
