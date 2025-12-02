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
 * sdcard.c
 *
 *  Created on: Nov 19, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"

#ifdef A_OS_SDCARD_ENABLED

#include "sdcard.h"
SDCARD_DriverStruct_t *sdcard_drv_ptr = NULL;

extern SD_HandleTypeDef hsd1;
#ifdef OLD
uint8_t SD_Init(void)
{
uint8_t sd_state = MSD_OK;
	MX_FATFS_Init();
	/* Check if the SD card is plugged in the slot */

	if (SD_IsDetected() != SD_PRESENT)
		return MSD_ERROR_SD_NOT_PRESENT;

	/* HAL SD initialization */
	sd_state = HAL_SD_Init(&hsd1);
	/* Configure SD Bus width (4 bits mode selected) */
	if (sd_state == MSD_OK)
	{
		/* Enable wide operation */
		if (HAL_SD_ConfigWideBusOperation(&hsd1, SDMMC_BUS_WIDE_4B) != HAL_OK)
			sd_state = MSD_ERROR;
	}
	return sd_state;
}
#endif
uint8_t SD_ITConfig(void)
{
  return (uint8_t)0;
}


uint8_t SD_ReadBlocks(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks, uint32_t Timeout)
{
uint8_t sd_state = MSD_OK;

	if (HAL_SD_ReadBlocks(&hsd1, (uint8_t *)pData, ReadAddr, NumOfBlocks, Timeout) != HAL_OK)
		sd_state = MSD_ERROR;
	return sd_state;
}

uint8_t SD_WriteBlocks(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks, uint32_t Timeout)
{
uint8_t sd_state = MSD_OK;

	if (HAL_SD_WriteBlocks(&hsd1, (uint8_t *)pData, WriteAddr, NumOfBlocks, Timeout) != HAL_OK)
		sd_state = MSD_ERROR;
	return sd_state;
}

uint8_t SD_ReadBlocks_DMA(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks)
{
uint8_t sd_state = MSD_OK;

	/* Read block(s) in DMA transfer mode */
	if (HAL_SD_ReadBlocks_DMA(&hsd1, (uint8_t *)pData, ReadAddr, NumOfBlocks) != HAL_OK)
		sd_state = MSD_ERROR;
	return sd_state;
}

uint8_t SD_WriteBlocks_DMA(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks)
{
uint8_t sd_state = MSD_OK;

	if (HAL_SD_WriteBlocks_DMA(&hsd1, (uint8_t *)pData, WriteAddr, NumOfBlocks) != HAL_OK)
		sd_state = MSD_ERROR;
	return sd_state;
}

uint8_t SD_Erase(uint32_t StartAddr, uint32_t EndAddr)
{
uint8_t sd_state = MSD_OK;

	if (HAL_SD_Erase(&hsd1, StartAddr, EndAddr) != HAL_OK)
		sd_state = MSD_ERROR;
	return sd_state;
}

uint8_t SD_GetCardState(void)
{
  return ((HAL_SD_GetCardState(&hsd1) == HAL_SD_CARD_TRANSFER ) ? SD_TRANSFER_OK : SD_TRANSFER_BUSY);
}

void HAL_SD_AbortCallback(SD_HandleTypeDef *hsd)
{
}

void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd)
{
	if ( sdcard_drv_ptr->hsd == hsd )
		WriteStatus = 1;
}

void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd)
{
	if ( sdcard_drv_ptr->hsd == hsd )
		ReadStatus = 1;
}

uint8_t SD_IsDetected(void)
{
SDCARD_DriverStruct_t *sdcard_Drv = sdcard_drv_ptr;
	if ( sdcard_Drv->sd_detect_port == NULL )
		return SD_NOT_PRESENT;

	if(HAL_GPIO_ReadPin(sdcard_Drv->sd_detect_port, sdcard_Drv->sd_detect_bit) != GPIO_PIN_RESET)
		return SD_NOT_PRESENT;
	return SD_PRESENT;
}

void SD_GetCardInfo(HAL_SD_CardInfoTypeDef *CardInfo)
{
SDCARD_DriverStruct_t *sdcard_Drv = sdcard_drv_ptr;
	if ( sdcard_Drv->sd_detect_port == NULL )
		return;
	HAL_SD_GetCardInfo(sdcard_Drv->hsd, CardInfo);
}

ITCM_AREA_CODE uint32_t	sdcard_register(SDCARD_DriverStruct_t *sdcard_Drv)
{
SDCARD_DriverStruct_t *eptr, *pre_eptr;

	if ( sdcard_Drv->sd_detect_port == NULL )
		return DRIVER_REQUEST_FAILED;

	if ( sdcard_Drv->hsd == NULL )
		return DRIVER_REQUEST_FAILED;
	if ( sdcard_drv_ptr == NULL)
	{
		sdcard_drv_ptr = sdcard_Drv;
		sdcard_Drv->next_drv = NULL;
	}
	else
	{
		eptr = pre_eptr = sdcard_drv_ptr;
		while(eptr->next_drv != NULL)
		{
			pre_eptr = eptr;
			eptr = (SDCARD_DriverStruct_t *)eptr->next_drv;
		}
		pre_eptr->next_drv = (uint32_t *)sdcard_Drv;
		sdcard_Drv->next_drv = NULL;
	}

	MX_FATFS_Init();
	/* Check if the SD card is plugged in the slot */

	if (SD_IsDetected() != SD_PRESENT)
		return 1;

	if (HAL_SD_Init(sdcard_Drv->hsd) == 0)
	{
		if (HAL_SD_ConfigWideBusOperation(&hsd1, SDMMC_BUS_WIDE_4B) != HAL_OK)
			return 1;
	}

	HAL_SD_GetCardCID(sdcard_Drv->hsd, &sdcard_Drv->pCID);
	HAL_SD_GetCardCSD(sdcard_Drv->hsd, &sdcard_Drv->pCSD);
	HAL_SD_GetCardInfo(sdcard_Drv->hsd, &sdcard_Drv->CardInfo);

	return 0;
}


#endif //#ifdef A_OS_SDCARD_ENABLED

