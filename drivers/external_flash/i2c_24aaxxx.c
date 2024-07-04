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
 * i2c_24aaxxx.c
 *
 *  Created on: Jul 4, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/A.h"
#include "../../kernel/scheduler.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/hwmanager.h"
#include "../../kernel/system_default.h"
#include "i2c_24aaxxx.h"

#ifdef I2CFLASH_ENABLED

uint16_t	i2cflash_device_address	= 0;
extern	HWMngr_t	HWMngr[PERIPHERAL_NUM];

uint8_t i2c_24aaxxx_init(uint32_t i2c,uint16_t dev_addr)
{
	i2cflash_device_address = dev_addr;
	if ( hw_i2c_check_presence(i2c,i2cflash_device_address) )
		return HW_I2C_ERROR_DEVICE_NOT_PRESENT;
	if ( allocate_hw(i2c,0) == 0 )
		return HW_I2C_ERROR_HW_NOT_OWNED;
	return 0;
}

uint32_t i2c_24aaxxx_read(uint32_t i2c,uint16_t addr,uint8_t* data, uint16_t size)
{
	if ( i2cflash_device_address == 0 )
		return HW_I2C_ERROR_HW_NOT_INITIALIZED;
	if ( HWMngr[i2c].process != Asys.current_process )
		return HW_I2C_ERROR_HW_NOT_OWNED;
	if ( size > I2C_24AAXXX_PAGE_SIZE )
		return HW_I2C_ERROR_SIZE_TOO_BIG;
	return hw_i2c_MemGet16(i2c, i2cflash_device_address,addr,data, size);
}

uint32_t i2c_24aaxxx_write(uint32_t i2c,uint16_t addr,uint8_t* data, uint16_t size)
{
	if ( i2cflash_device_address == 0 )
		return HW_I2C_ERROR_HW_NOT_INITIALIZED;
	if ( HWMngr[i2c].process != Asys.current_process )
		return HW_I2C_ERROR_HW_NOT_OWNED;
	if ( size > I2C_24AAXXX_PAGE_SIZE )
		return HW_I2C_ERROR_SIZE_TOO_BIG;
	return hw_i2c_MemSend16(i2c,i2cflash_device_address,addr,data, size);

}

#endif
