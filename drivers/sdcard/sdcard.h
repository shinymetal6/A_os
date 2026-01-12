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
 *  Created on: Nov 19, 2025
 *      Author: fil
 */

#ifndef DRIVERS_SDCARD_SDCARD_H_
#define DRIVERS_SDCARD_SDCARD_H_
#ifdef A_OS_SDCARD_ENABLED

#ifdef __cplusplus
 extern "C" {
#endif

#define   	MSD_OK                        ((uint8_t)0x00)
#define   	MSD_ERROR                     ((uint8_t)0x01)
#define   	MSD_ERROR_SD_NOT_PRESENT      ((uint8_t)0x02)

#define   	SD_TRANSFER_OK                ((uint8_t)0x00)
#define   	SD_TRANSFER_BUSY              ((uint8_t)0x01)

#define 	SD_PRESENT               ((uint8_t)0x01)
#define 	SD_NOT_PRESENT           ((uint8_t)0x00)
#define 	SD_DATATIMEOUT           ((uint32_t)100000000)

#define SD_DetectIRQHandler()             HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8)

typedef struct
{
	/* driver header */
	uint8_t				status;
	uint8_t				flags;
	uint8_t 			process;
	SD_HandleTypeDef 	*hsd;
	GPIO_TypeDef	 	*sd_detect_port;
	uint16_t			sd_detect_bit;
	uint32_t 			wakeup_id;
	uint32_t			*next_drv;
	HAL_SD_CardCIDTypedef pCID;
	HAL_SD_CardCSDTypedef pCSD;
	HAL_SD_CardInfoTypeDef CardInfo;
}SDCARD_DriverStruct_t;
/* status */
#define	SDCARD_DMA_WRITE_COMPLETE	0x80
#define	SDCARD_DMA_READ_COMPLETE	0x40

uint8_t SD_Init(void);
uint8_t SD_ITConfig(void);
uint8_t SD_ReadBlocks(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks, uint32_t Timeout);
uint8_t SD_WriteBlocks(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks, uint32_t Timeout);
uint8_t SD_ReadBlocks_DMA(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks);
uint8_t SD_WriteBlocks_DMA(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks);
uint8_t SD_Erase(uint32_t StartAddr, uint32_t EndAddr);
uint8_t SD_GetCardState(void);
void    SD_GetCardInfo(HAL_SD_CardInfoTypeDef *CardInfo);
uint8_t SD_IsDetected(void);

/* These functions can be modified in case the current settings (e.g. DMA stream)
   need to be changed for specific application needs */
void    BSP_SD_AbortCallback(void);
void    BSP_SD_WriteCpltCallback(void);
void    BSP_SD_ReadCpltCallback(void);

extern	uint8_t  WriteStatus, ReadStatus;

extern	uint32_t	sdcard_register(SDCARD_DriverStruct_t *sdcard_Drv);

#ifdef __cplusplus
}
#endif
#endif //#ifdef A_OS_SDCARD_ENABLED

#endif /* DRIVERS_SDCARD_SDCARD_H_ */
