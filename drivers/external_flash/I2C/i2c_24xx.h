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
 * i2c_24xx.h
 *
 *  Created on: Nov 4, 2024
 *      Author: fil
 */
#ifndef DRIVERS_EXTERNAL_FLASH_I2C_24XX_H_
#define DRIVERS_EXTERNAL_FLASH_I2C_24XX_H_

#define	I2C_24XX_STOPPED	0
#define	I2C_24XX_ADDRESS	0xa0
#define	I2C_24XX_PAGESIZE	256
#define	I2C_24XX_TIMEOUT	20

typedef struct
{
	uint8_t				status;
	uint8_t				flags;
	uint8_t				*data;
	I2C_HandleTypeDef 	*bus;
	uint16_t 			device_address;
	uint8_t 			device_address_size;
	GPIO_TypeDef	 	*power_port;
	uint16_t			power_bit;
	uint16_t			power_active_level;
	uint32_t 			wakeup_id;
	uint32_t 			timeout;
}I2C_24xx_Drv_TypeDef;

/* status */
#define	I2C_BUSY					0x08
#define	I2C_READ_COMPLETE			0x04
#define	I2C_WRITE_COMPLETE			0x02
#define	I2C_ERROR					0x01
/* flags */
#define	I2C_USES_WRITE_DMA			0x80
#define	I2C_USES_READ_DMA			0x40
#define	I2C_WAKEUP_ON_READ			0x20
#define	I2C_WAKEUP_ON_WRITE			0x10

extern uint32_t	i2c_24xx_register(I2C_24xx_Drv_TypeDef *driver_private_data);
extern uint32_t i2c_24xx_get_status(uint8_t handle);
extern uint32_t i2c_24xx_read(uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len);
extern uint32_t i2c_24xx_read_config(uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len);
extern uint32_t i2c_24xx_write(uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len);
extern uint32_t i2c_24xx_init(uint8_t handle);

#endif /* DRIVERS_EXTERNAL_FLASH_I2C_24XX_H_ */
