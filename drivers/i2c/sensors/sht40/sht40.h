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
 * sht40.h
 *
 *  Created on: Mar 13, 2024
 *      Author: fil
 */

#ifndef DRIVERS_SENSORS_SHT40_SHT40_H_
#define DRIVERS_SENSORS_SHT40_SHT40_H_

typedef struct
{
	uint8_t				status;
	uint8_t				flags;
	uint8_t				*data;
	I2C_HandleTypeDef 	*bus;
	uint16_t 			device_address;
	uint16_t 			device_address_size;
	GPIO_TypeDef	 	*power_port;
	uint16_t			power_bit;
	uint16_t			power_active_level;
	uint32_t 			wakeup_id;
}Sht40_Drv_TypeDef;
/* status */
#define	SHT40_STARTED		0x80
#define	SHT40_STARTED		0x80
#define	SHT40_STATUS_WRITE_COMPLETE		0x80
#define	SHT40_STATUS_ERROR				0x80
#define	SHT40_STATUS_BUSY				0x00

#define	SHT40_ADDR					0x88

#define	SHT40_DATA_HP		0xfd		/* reply 6 bytes 	measure T & RH with high precision (high repeatability) 	*/
#define	SHT40_DATA_MP		0xf6		/* reply 6 bytes  	measure T & RH with high precision (medium repeatability) 	*/
#define	SHT40_DATA_LP		0xe0		/* reply 6 bytes  	measure T & RH with high precision (low repeatability) 	*/
#define	SHT40_UUID			0x89		/* reply 2 bytes  	read serial number 	*/
#define	SHT40_SWRESET		0x94		/* reply 1 byte  	soft reset 	*/
#define	SHT40_HEAT200_1		0x39		/* reply 6 bytes  	heater ON 1 s   200mW and measure T & RH with high precision (high repeatability)  	*/
#define	SHT40_HEAT200_01	0x32		/* reply 6 bytes  	heater ON 0.1 s 200mW and measure T & RH with high precision (high repeatability)  	*/
#define	SHT40_HEAT110_1		0x2f		/* reply 6 bytes  	heater ON 1 s   110mW and measure T & RH with high precision (high repeatability)  	*/
#define	SHT40_HEAT110_01	0x24		/* reply 6 bytes  	heater ON 0.1 s 110mW and measure T & RH with high precision (high repeatability)  	*/
#define	SHT40_HEAT20_1		0x39		/* reply 6 bytes  	heater ON 1 s    20mW and measure T & RH with high precision (high repeatability)  	*/
#define	SHT40_HEAT20_01		0x32		/* reply 6 bytes  	heater ON 0.1 s  20mW and measure T & RH with high precision (high repeatability)  	*/

#define SHT40_I2C_TIMEOUT			1000U
#define SHT40_DRIVER_NOT_OWNED		0xffffffff

extern	uint32_t sht40_register(I2C_Sensors_DriverStruct_t *driver_private_data);

#endif /* DRIVERS_SENSORS_SHT40_SHT40_H_ */
