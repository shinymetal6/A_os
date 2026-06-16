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
 * mcp45xx.c
 *
 *  Created on: Mar 23, 2026
 *      Author: fil
 */


#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef A_OS_I2C_ENABLED
#include "../i2c.h"
#include "mcp45xx.h"

ITCM_AREA_CODE static uint32_t mcp45xx_SetTCON(I2C_MCP45xx_Drv_TypeDef *mcp45xx_Drv)
{
uint8_t buffer[2] = {0,0};

	if ( mcp45xx_Drv->dev_found == 0 )
		return 1;
	buffer[0] = (MCP45XX_VOLATILE_TCON << 4) | MCP45_WRITE_COMMAND;
	buffer[1] = MCP45XX_TCON_ENABLE;

	if ( HAL_I2C_Master_Transmit(mcp45xx_Drv->bus, mcp45xx_Drv->device_address, buffer, 2, MCP45XX_I2C_TIMEOUT) )
		return 1;
	return 0;
}

ITCM_AREA_CODE uint32_t mcp45xx_SetWiper(I2C_MCP45xx_Drv_TypeDef *mcp45xx_Drv,uint8_t value)
{
uint8_t buffer[2] = {0,0};

	if ( mcp45xx_Drv->dev_found == 0 )
		return 1;
	buffer[0] = (MCP45XX_VOLATILE_WIPER << 4) | MCP45_WRITE_COMMAND;
	buffer[1] = value;

	if ( HAL_I2C_Master_Transmit(mcp45xx_Drv->bus, mcp45xx_Drv->device_address, buffer, 2, MCP45XX_I2C_TIMEOUT) )
		return 1;
	return 0;
}

ITCM_AREA_CODE uint32_t mcp45xx_IncrementWiper(I2C_MCP45xx_Drv_TypeDef *mcp45xx_Drv)
{
uint8_t value;

	if ( mcp45xx_Drv->dev_found == 0 )
		return 1;
	value = (MCP45XX_VOLATILE_WIPER << 4) | MCP45_INCREMENT_WIPER;

	if ( HAL_I2C_Master_Transmit(mcp45xx_Drv->bus, mcp45xx_Drv->device_address, &value, 1, MCP45XX_I2C_TIMEOUT) )
		return 1;
	return 0;
}

ITCM_AREA_CODE uint32_t mcp45xx_DecrementWiper(I2C_MCP45xx_Drv_TypeDef *mcp45xx_Drv)
{
uint8_t value;

	if ( mcp45xx_Drv->dev_found == 0 )
		return 1;
	value = (MCP45XX_VOLATILE_WIPER << 4) | MCP45_DEREMENT_WIPER;

	if ( HAL_I2C_Master_Transmit(mcp45xx_Drv->bus, mcp45xx_Drv->device_address, &value, 1, MCP45XX_I2C_TIMEOUT) )
		return 1;
	return 0;
}

ITCM_AREA_CODE uint32_t mcp45xx_GetWiper(I2C_MCP45xx_Drv_TypeDef *mcp45xx_Drv)
{
uint8_t cmd;
uint8_t buffer[2] = {0,0};

	if ( mcp45xx_Drv->dev_found == 0 )
		return 1;
	cmd = (MCP45XX_VOLATILE_WIPER << 4) | MCP45_READ_WIPER;
	if ( HAL_I2C_Master_Transmit(mcp45xx_Drv->bus, mcp45xx_Drv->device_address, &cmd, 1, 100) )
		return 1;

	if ( HAL_I2C_Master_Receive(mcp45xx_Drv->bus, mcp45xx_Drv->device_address, buffer, 2, 100) )
		return 1;
	mcp45xx_Drv->value = buffer[1];
	return buffer[1];
}

ITCM_AREA_CODE uint32_t mcp45xx_ShutDown(I2C_MCP45xx_Drv_TypeDef *mcp45xx_Drv)
{
	HAL_GPIO_WritePin(mcp45xx_Drv->shutdown_port,mcp45xx_Drv->shutdown_bit, GPIO_PIN_RESET);
	return 0;
}

ITCM_AREA_CODE uint32_t mcp45xx_PowerUp(I2C_MCP45xx_Drv_TypeDef *mcp45xx_Drv)
{
	HAL_GPIO_WritePin(mcp45xx_Drv->shutdown_port,mcp45xx_Drv->shutdown_bit, GPIO_PIN_SET);
	return 0;
}

ITCM_AREA_CODE uint32_t mcp45xx_WlatLow(I2C_MCP45xx_Drv_TypeDef *mcp45xx_Drv)
{
	HAL_GPIO_WritePin(mcp45xx_Drv->wlat_port,mcp45xx_Drv->wlat_bit, GPIO_PIN_RESET);
	return 0;
}

ITCM_AREA_CODE uint32_t mcp45xx_WlatHigh(I2C_MCP45xx_Drv_TypeDef *mcp45xx_Drv)
{
	HAL_GPIO_WritePin(mcp45xx_Drv->wlat_port,mcp45xx_Drv->wlat_bit, GPIO_PIN_SET);
	return 0;
}


ITCM_AREA_CODE uint32_t mcp45xx_register(I2C_MCP45xx_Drv_TypeDef *mcp45xx_Drv)
{
I2C_DriverStruct_t *eptr, *pre_eptr;

	mcp45xx_Drv->dev_found = 0;
	if ( mcp45xx_Drv->bus == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( mcp45xx_Drv->wlat_port == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( mcp45xx_Drv->shutdown_port == NULL)
		return DRIVER_REQUEST_FAILED;

	mcp45xx_PowerUp(mcp45xx_Drv);
	mcp45xx_WlatHigh(mcp45xx_Drv);

    if (HAL_I2C_IsDeviceReady(mcp45xx_Drv->bus, mcp45xx_Drv->device_address, 2, MCP45XX_I2C_TIMEOUT))
		return DRIVER_REQUEST_FAILED;

	if ( i2c_drv_ptr == NULL)
	{
		i2c_drv_ptr = (I2C_DriverStruct_t *)mcp45xx_Drv;
		mcp45xx_Drv->next_drv = NULL;
	}
	else
	{
		eptr = pre_eptr = i2c_drv_ptr;
		while(eptr->next_drv != NULL)
		{
			pre_eptr = eptr;
			eptr = (I2C_DriverStruct_t *)eptr->next_drv;
		}
		pre_eptr->next_drv = (uint32_t *)mcp45xx_Drv;
		mcp45xx_Drv->next_drv = NULL;
	}
	mcp45xx_Drv->process = get_current_process();
	mcp45xx_Drv->dev_found = 1;
	mcp45xx_SetTCON(mcp45xx_Drv);
	return 0;
}
#endif // #ifdef A_OS_I2C_ENABLED

