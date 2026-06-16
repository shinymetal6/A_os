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
 * mcp4651.c
 *
 *  Created on: Jun 15, 2026
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef A_OS_I2C_ENABLED
#include "../i2c.h"
#include "mcp4651.h"

ITCM_AREA_CODE	uint32_t MCP4651_SetWiper(I2C_MCP4651_Drv_TypeDef *mcp4651_Drv, uint8_t wiper, uint8_t value)
{
uint8_t tx_buffer[2];
uint8_t cmd_byte = (wiper == 0) ? MCP4651_CMD_WRITE_DATA | (MCP4651_POT_WIPER_0<<4) : MCP4651_CMD_WRITE_DATA | (MCP4651_POT_WIPER_1<<4);

    tx_buffer[0] = cmd_byte;
    tx_buffer[1] = value;
    return HAL_I2C_Master_Transmit(mcp4651_Drv->bus, mcp4651_Drv->device_address, tx_buffer, 2, mcp4651_Drv->timeout);
}

ITCM_AREA_CODE	uint32_t MCP4651_GetWiper(I2C_MCP4651_Drv_TypeDef *mcp4651_Drv, uint8_t wiper, uint8_t *value)
{
uint8_t cmd_byte = (wiper == 0) ? MCP4651_CMD_READ_DATA | (MCP4651_POT_WIPER_0<<4) : MCP4651_CMD_WRITE_DATA | (MCP4651_POT_WIPER_1<<4);

    if (HAL_I2C_Master_Transmit(mcp4651_Drv->bus, mcp4651_Drv->device_address, &cmd_byte, 1, mcp4651_Drv->timeout) != HAL_OK)
        return 1;
    return HAL_I2C_Master_Receive(mcp4651_Drv->bus, mcp4651_Drv->device_address, value, 1, mcp4651_Drv->timeout);
}

ITCM_AREA_CODE uint32_t mcp4651_register(I2C_MCP4651_Drv_TypeDef *mcp4651_Drv)
{
I2C_DriverStruct_t *eptr, *pre_eptr;

	mcp4651_Drv->dev_found = 0;
	if ( mcp4651_Drv->bus == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( mcp4651_Drv->device_address == 0)
		return DRIVER_REQUEST_FAILED;
	if ( mcp4651_Drv->timeout == 0)
		mcp4651_Drv->timeout = MCP4651_I2C_TIMEOUT;
    if (HAL_I2C_IsDeviceReady(mcp4651_Drv->bus, mcp4651_Drv->device_address, 2, mcp4651_Drv->timeout))
		return DRIVER_REQUEST_FAILED;

	if ( i2c_drv_ptr == NULL)
	{
		i2c_drv_ptr = (I2C_DriverStruct_t *)mcp4651_Drv;
		mcp4651_Drv->next_drv = NULL;
	}
	else
	{
		eptr = pre_eptr = i2c_drv_ptr;
		while(eptr->next_drv != NULL)
		{
			pre_eptr = eptr;
			eptr = (I2C_DriverStruct_t *)eptr->next_drv;
		}
		pre_eptr->next_drv = (uint32_t *)mcp4651_Drv;
		mcp4651_Drv->next_drv = NULL;
	}
	mcp4651_Drv->process = get_current_process();
	mcp4651_Drv->dev_found = 1;
	return 0;
}

#endif // #ifdef A_OS_I2C_ENABLED


