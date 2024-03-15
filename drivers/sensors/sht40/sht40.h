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

#define	SHT40_I2C_TIMEOUT	1000
#define	SHT40_ADDR			0x88

/* measure data format
	[2 * 8-bit T-data; 8-bit CRC; 2 * 8-bit RH-data; 8-bit CRC]
*/

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

extern	int32_t  SHT40_Start_HP_Acquisition(void);
extern	int32_t  SHT40_Start_MP_Acquisition(void);
extern	int32_t  SHT40_Start_LP_Acquisition(void);

extern	int32_t  SHT40_Start_Heat200_1sec_Acquisition(void);
extern	int32_t  SHT40_Start_Heat200_01sec_Acquisition(void);
extern	int32_t  SHT40_Start_Heat110_1sec_Acquisition(void);
extern	int32_t  SHT40_Start_Heat110_01sec_Acquisition(void);
extern	int32_t  SHT40_Start_Heat20_1sec_Acquisition(void);
extern	int32_t  SHT40_Start_Heat20_01sec_Acquisition(void);

extern	int32_t  SHT40_ReadData(uint8_t *pData);
extern	int32_t  SHT40_Get_uid(uint8_t *pData);


#endif /* DRIVERS_SENSORS_SHT40_SHT40_H_ */
