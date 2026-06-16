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
 * mcp45xx.h
 *
 *  Created on: Mar 23, 2026
 *      Author: fil
 */

#ifndef DRIVERS_I2C_E2POT_MCP45XX_H_
#define DRIVERS_I2C_E2POT_MCP45XX_H_

#define	MCP45XX_I2C_TIMEOUT				100
#define MCP45XX_MAX_VALUE     			127

// Command Codes
#define MCP45XX_VOLATILE_WIPER			0x00
#define MCP45XX_VOLATILE_TCON			0x04
#define MCP45XX_TCON_ENABLE				0xfb

#define MCP45_WRITE_COMMAND				0x00
#define MCP45_INCREMENT_WIPER			0x04
#define MCP45_DEREMENT_WIPER			0x08
#define MCP45_READ_WIPER				0x0C

#define MCP45_I2C_ADDR					0x3C

#define MCP45_I2C_ADDR_0				((MCP45_I2C_ADDR + 0x00) << 1)
#define MCP45_I2C_ADDR_1				((MCP45_I2C_ADDR + 0x01) << 1)
#define MCP45_I2C_ADDR_2				((MCP45_I2C_ADDR + 0x02) << 1)
#define MCP45_I2C_ADDR_3				((MCP45_I2C_ADDR + 0x03) << 1)
typedef struct
{
	/* driver header */
	uint8_t				status;
	uint8_t				flags;
	uint8_t 			process;
	I2C_HandleTypeDef 	*bus;
	uint16_t 			device_address;
	uint32_t 			wakeup_id;
	I2C_DriverStruct_t	*next_drv;
	/* driver proprietary data */
	GPIO_TypeDef	 	*i2c_scl_port;
	uint16_t			i2c_scl_bit;
	GPIO_TypeDef	 	*wlat_port;
	uint16_t			wlat_bit;
	GPIO_TypeDef	 	*shutdown_port;
	uint16_t			shutdown_bit;
	uint32_t 			timeout;
	uint8_t				value;
	uint8_t				dev_found;
}I2C_MCP45xx_Drv_TypeDef;

extern uint32_t mcp45xx_register(I2C_MCP45xx_Drv_TypeDef *mcp45xx_Drv);
extern uint32_t mcp45xx_GetWiper(I2C_MCP45xx_Drv_TypeDef *mcp45xx_Drv);
extern uint32_t mcp45xx_SetWiper(I2C_MCP45xx_Drv_TypeDef *mcp45xx_Drv,uint8_t value);
extern uint32_t mcp45xx_IncrementWiper(I2C_MCP45xx_Drv_TypeDef *mcp45xx_Drv);
extern uint32_t mcp45xx_DecrementWiper(I2C_MCP45xx_Drv_TypeDef *mcp45xx_Drv);

extern uint32_t mcp45xx_ShutDown(I2C_MCP45xx_Drv_TypeDef *mcp45xx_Drv);
extern uint32_t mcp45xx_PowerUp(I2C_MCP45xx_Drv_TypeDef *mcp45xx_Drv);
extern uint32_t mcp45xx_WlatLow(I2C_MCP45xx_Drv_TypeDef *mcp45xx_Drv);
extern uint32_t mcp45xx_WlatHigh(I2C_MCP45xx_Drv_TypeDef *mcp45xx_Drv);

#endif /* DRIVERS_I2C_E2POT_MCP45XX_H_ */
