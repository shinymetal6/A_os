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
 * sdcard.h
 *
 *  Created on: Nov 14, 2025
 *      Author: fil
 */

#ifndef DRIVERS_SDCARD_SDCARD_H_
#define DRIVERS_SDCARD_SDCARD_H_

#ifdef A_OS_SDCARD_ENABLED
#include "../../modules/fat/fat.h"

extern uint8_t retSD; /* Return value for SD */
extern char SDPath[4]; /* SD logical drive path */
extern FATFS SDFatFS; /* File system object for SD logical drive */
extern FIL SDFile; /* File object for SD */

typedef struct
{
	uint8_t 			process;
	uint8_t				status;
	uint8_t				flags;
	uint16_t			sd_detect_bit;
	GPIO_TypeDef	 	*sd_detect_port;
}SDCARD_DriverStruct_t;

extern	void 	MX_FATFS_Init(void);
extern	uint8_t AOS_SD_Register(SDCARD_DriverStruct_t *sdcard_drv);

extern	uint8_t AOS_SD_GetCardState(void);
extern	uint8_t AOS_SD_ReadBlocks_DMA(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks);
extern	uint8_t AOS_SD_WriteBlocks_DMA(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks);
extern	void AOS_SD_GetCardInfo(HAL_SD_CardInfoTypeDef *CardInfo);

#endif //#ifdef A_OS_SDCARD_ENABLED

#endif /* DRIVERS_SDCARD_SDCARD_H_ */
