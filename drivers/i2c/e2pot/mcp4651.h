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
 * mcp4651.h
 *
 *  Created on: Jun 15, 2026
 *      Author: fil
 */

#ifndef DRIVERS_I2C_E2POT_MCP4651_H_
#define DRIVERS_I2C_E2POT_MCP4651_H_

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
	uint32_t 			timeout;
	uint8_t				value;
	uint8_t				dev_found;
}I2C_MCP4651_Drv_TypeDef;

#define MCP4651_I2C_ADDR        0x50
#define	MCP4651_I2C_TIMEOUT		100

// Command Codes (Bits 7-4)
#define MCP4651_CMD_WRITE_DATA      0x00
#define MCP4651_CMD_WRITE_NV        0x02
#define MCP4651_CMD_INCREMENT       0x04
#define MCP4651_CMD_DECREMENT       0x05
#define MCP4651_CMD_READ_DATA       0x08
#define MCP4651_CMD_READ_NV         0x09
#define MCP4651_CMD_READ_STATUS     0x0D
#define MCP4651_CMD_READ_TCON       0x0E

// Potentiometer Select Codes (Bits 3-0)
#define MCP4651_POT_WIPER_0         0x00
#define MCP4651_POT_WIPER_1         0x01
#define MCP4651_POT_NV_WIPER_0      0x02
#define MCP4651_POT_NV_WIPER_1      0x03
#define MCP4651_POT_TCON            0x04
#define MCP4651_POT_STATUS          0x05

//#define MCP4651_BUILD_CMD(cmd, pot) (((cmd) << 4) | (pot))

extern 	uint32_t mcp4651_register(I2C_MCP4651_Drv_TypeDef *mcp4651_Drv);
extern	uint32_t MCP4651_SetWiper(I2C_MCP4651_Drv_TypeDef *mcp4651_Drv, uint8_t wiper, uint8_t value);
extern	uint32_t MCP4651_GetWiper(I2C_MCP4651_Drv_TypeDef *mcp4651_Drv, uint8_t wiper, uint8_t *value);


#endif /* DRIVERS_I2C_E2POT_MCP4651_H_ */
