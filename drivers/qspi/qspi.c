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
 * qspi.c
 *
 *  Created on: Nov 17, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "qspi_w25q128.h"

QSPI_DriverStruct_t 	*qspi_drv_ptr;

ITCM_AREA_CODE uint32_t	qspi_read(QSPI_DriverStruct_t *qspi_Drv, uint32_t address,uint8_t *data,uint32_t data_len)
{
	if ( qspi_Drv->read != NULL)
		return qspi_Drv->read((uint32_t *)qspi_Drv, address,data,data_len);
	return 1;
}

ITCM_AREA_CODE uint32_t	qspi_write(QSPI_DriverStruct_t *qspi_Drv, uint32_t address,uint8_t *data,uint32_t data_len)
{
	if ( qspi_Drv->write != NULL)
		return qspi_Drv->write((uint32_t *)qspi_Drv, address,data,data_len);
	return 1;
}

ITCM_AREA_CODE uint32_t	qspi_erase_sectors(QSPI_DriverStruct_t *qspi_Drv,uint32_t sector_addr)
{
	if ( qspi_Drv->erase_sector != NULL)
		return qspi_Drv->erase_sector((uint32_t *)qspi_Drv, sector_addr);
	return 1;
}

ITCM_AREA_CODE uint32_t	qspi_get_id(QSPI_DriverStruct_t *qspi_Drv)
{
	if ( qspi_Drv->get_id != NULL)
		return qspi_Drv->get_id((uint32_t *)qspi_Drv);
	return 1;
}

ITCM_AREA_CODE uint32_t	qspi_chip_erase(QSPI_DriverStruct_t *qspi_Drv)
{
	if ( qspi_Drv->chip_erase != NULL)
		return qspi_Drv->chip_erase((uint32_t *)qspi_Drv);
	return 1;
}

ITCM_AREA_CODE uint32_t	qspi_memory_map(QSPI_DriverStruct_t *qspi_Drv)
{
	if ( qspi_Drv->memory_map != NULL)
		return qspi_Drv->memory_map((uint32_t *)qspi_Drv);
	return 1;
}

ITCM_AREA_CODE uint32_t	qspi_reset_chip(QSPI_DriverStruct_t *qspi_Drv)
{
	if ( qspi_Drv->chip_reset != NULL)
		return qspi_Drv->chip_reset((uint32_t *)qspi_Drv);
	return 1;
}


ITCM_AREA_CODE uint32_t	qspi_register(QSPI_DriverStruct_t *qspi_Drv)
{
QSPI_DriverStruct_t *eptr, *pre_eptr;

	if ( qspi_Drv->wakeup_id == 0 )
		return DRIVER_REQUEST_FAILED;
	if ( qspi_Drv->qspi == NULL )
		return DRIVER_REQUEST_FAILED;
	if ( qspi_Drv->qspi_id == 0)
		return DRIVER_REQUEST_FAILED;

	if ( qspi_drv_ptr == NULL)
	{
		qspi_drv_ptr = qspi_Drv;
		qspi_Drv->next_drv = NULL;
	}
	else
	{
		eptr = pre_eptr = (QSPI_DriverStruct_t *)qspi_drv_ptr;
		while(eptr->next_drv != NULL)
		{
			pre_eptr = eptr;
			eptr = (QSPI_DriverStruct_t *)eptr->next_drv;
		}
		pre_eptr->next_drv = (uint32_t *)qspi_drv_ptr;
		qspi_Drv->next_drv = NULL;
	}
	qspi_Drv->process = get_current_process();
	if ( qspi_Drv->qspi_id == QSPI_25XX)
	{
		qspi_Drv->read = W25Q128JV_FastReadQuad;
		qspi_Drv->write = W25Q128JV_PagesProgram;
		qspi_Drv->erase_sector = W25Q128JV_EraseSector;
		qspi_Drv->get_id = W25Q128JV_ReadID;
		qspi_Drv->memory_map = W25Q128JV_EnableMemoryMappedMode;
		qspi_Drv->chip_erase = W25Q128JV_ChipErase;
		qspi_Drv->chip_reset = W25Q128JV_ResetChip;
	}
    bzero((uint8_t *)&qspi_Drv->cmd,sizeof(QSPI_CommandTypeDef));

	return 0;
}

void HAL_QSPI_TxCpltCallback(QSPI_HandleTypeDef *hqspi)
{
QSPI_DriverStruct_t *w25qxx_Drv = qspi_drv_ptr;
	w25qxx_Drv->status |= QSPI_DMA_WRITE_COMPLETE;
	if ( w25qxx_Drv->flags & QSPI_FLAGS_WAKEUP)
		activate_process(w25qxx_Drv->process,EVENT_QSPI_IRQ,HW_QSPI);
}

void HAL_QSPI_RxCpltCallback(QSPI_HandleTypeDef *hqspi)
{
QSPI_DriverStruct_t *w25qxx_Drv = qspi_drv_ptr;
	w25qxx_Drv->status |= QSPI_DMA_READ_COMPLETE;
}

void HAL_QSPI_ErrorCallback(QSPI_HandleTypeDef *hqspi)
{
QSPI_DriverStruct_t *w25qxx_Drv = qspi_drv_ptr;
	w25qxx_Drv->status |= QSPI_DMA_ERROR;
}

