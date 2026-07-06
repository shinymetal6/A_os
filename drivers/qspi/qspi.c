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
 *  Created on: Jan 8, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#ifdef A_OS_QUADSPI_ENABLED

QSPI_DriverStruct_t 	*qspi_drv_ptr;


uint32_t qspi_read(QSPI_DriverStruct_t *qspi_Drv, uint32_t address,uint8_t *data,uint32_t data_len)
{
	if ( qspi_Drv->read != NULL )
		return qspi_Drv->read((uint32_t *)qspi_Drv,address,data,data_len);
	return 1;
}

uint32_t qspi_write(QSPI_DriverStruct_t *qspi_Drv, uint32_t address,uint8_t *data,uint32_t data_len)
{
	if ( qspi_Drv->write != NULL )
		return qspi_Drv->write((uint32_t *)qspi_Drv,address,data,data_len);
	return 1;
}

uint32_t qspi_erase_blocks(QSPI_DriverStruct_t *qspi_Drv, uint32_t start_block,uint32_t number_of_blocks)
{
	if ( qspi_Drv->erase_blocks != NULL )
		return qspi_Drv->erase_blocks((uint32_t *)qspi_Drv,start_block,number_of_blocks);
	return 1;
}

uint32_t qspi_erase_sectors(QSPI_DriverStruct_t *qspi_Drv, uint32_t start_sector,uint32_t number_of_sectors)
{
	if ( qspi_Drv->erase_sectors != NULL )
		return qspi_Drv->erase_sectors((uint32_t *)qspi_Drv,start_sector,number_of_sectors);
	return 1;
}

uint32_t qspi_erase_chip(QSPI_DriverStruct_t *qspi_Drv)
{
	if ( qspi_Drv->erase_chip != NULL )
		return qspi_Drv->erase_chip((uint32_t *)qspi_Drv);
	return 1;
}

uint32_t qspi_get_id(QSPI_DriverStruct_t *qspi_Drv,uint8_t *data)
{
	if ( qspi_Drv->get_id != NULL )
		return qspi_Drv->get_id((uint32_t *)qspi_Drv,data);
	return 1;
}

ITCM_AREA_CODE uint32_t	qspi_register(QSPI_DriverStruct_t *qspi_Drv)
{
QSPI_DriverStruct_t *eptr;
uint8_t id = 0;

	if ( qspi_Drv->wakeup_id == 0 )
		return DRIVER_REQUEST_FAILED;
	if ( qspi_Drv->qspi_bus == NULL )
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
		eptr = (QSPI_DriverStruct_t *)qspi_drv_ptr;
		while(eptr->next_drv != NULL)
			eptr = (QSPI_DriverStruct_t *)eptr->next_drv;
		eptr->next_drv = (uint32_t *)qspi_drv_ptr;
		qspi_Drv->next_drv = NULL;
	}
	qspi_Drv->process = get_current_process();

	if ( qspi_Drv->qspi_id == QSPI_25XX)
	{
		qspi_Drv->read = w25qxx_read;
		qspi_Drv->write = w25qxx_write;
		qspi_Drv->erase_sectors = w25qxx_erasesectors;
		qspi_Drv->erase_blocks = w25qxx_eraseblocks;
		qspi_Drv->erase_chip = w25qxx_erasechip;
		qspi_Drv->read_status_register = w25qxx_ReadStatusReg;
		qspi_Drv->write_status_register = w25qxx_WriteStatusReg;
		qspi_Drv->read_all_status_register = w25qxx_ReadAllStatusRegs;
		qspi_Drv->get_id = w25qxx_GetID;
	}

	if ( qspi_Drv->qspi_bus->hmdma == NULL )
	{
		// disable dma if it's not configured in hw
		qspi_Drv->flags &= ~QSPI_USES_DMA;
	}

	qspi_Drv->com.InstructionMode = 0;
	qspi_Drv->com.Instruction = 0;	 				// Command
	qspi_Drv->com.AddressMode = 0;
	qspi_Drv->com.AddressSize = qspi_Drv->FlashSize > 128U ? QSPI_ADDRESS_32_BITS : QSPI_ADDRESS_24_BITS;
	qspi_Drv->com.Address = 0;
	qspi_Drv->com.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	qspi_Drv->com.AlternateBytes = QSPI_ALTERNATE_BYTES_NONE;
	qspi_Drv->com.AlternateBytesSize = QSPI_ALTERNATE_BYTES_NONE;
	qspi_Drv->com.DataMode = 0;
	qspi_Drv->com.NbData = 0;
	qspi_Drv->com.DdrMode = QSPI_DDR_MODE_DISABLE;
	qspi_Drv->com.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	qspi_Drv->com.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
	qspi_Drv->com.DummyCycles = W25Q_DUMMY_0;

	qspi_Drv->status = DRIVER_STATUS_IN_USE;

	qspi_Drv->read_status_register((uint32_t *)qspi_Drv, 1,W25Q_READ_TIMEOUT);
	qspi_Drv->qspi_status_reg &= ~W25Q_NO_PROTECTION_MASK;
	qspi_Drv->write_status_register((uint32_t *)qspi_Drv, 1,W25Q_WRITE_TIMEOUT);

	qspi_Drv->read_status_register((uint32_t *)qspi_Drv, 2,W25Q_READ_TIMEOUT);
	qspi_Drv->qspi_status_reg |= 0x02;
	qspi_Drv->write_status_register((uint32_t *)qspi_Drv, 2,W25Q_WRITE_TIMEOUT);
	qspi_Drv->read_status_register((uint32_t *)qspi_Drv, 2,W25Q_READ_TIMEOUT);

	qspi_Drv->read_all_status_register((uint32_t *)qspi_Drv,W25Q_READ_TIMEOUT);
	qspi_Drv->get_id((uint32_t *)qspi_Drv,&id);
	return 0;
}


QSPI_DriverStruct_t	*qspi_irq_common(QSPI_HandleTypeDef *hqspi)
{
QSPI_DriverStruct_t	*qspi_drv_ptr_L = qspi_drv_ptr;
	if ( qspi_drv_ptr == NULL )
		return NULL;
	while(qspi_drv_ptr_L->qspi_bus != hqspi)
	{
		if ( qspi_drv_ptr_L->next_drv != NULL )
			qspi_drv_ptr_L = (QSPI_DriverStruct_t *)qspi_drv_ptr->next_drv;
	}
	if (qspi_drv_ptr_L != NULL)
		if ( qspi_drv_ptr_L->process != 0 )
			return qspi_drv_ptr_L;
	return NULL;
}

void HAL_QSPI_RxCpltCallback(QSPI_HandleTypeDef *hqspi)
{
QSPI_DriverStruct_t	*qspi_Drv;

	__disable_irq();
	qspi_Drv = qspi_irq_common(hqspi);
	if ( qspi_Drv != NULL )
	{
		qspi_Drv->status |= QSPI_READ_COMPLETE;
		activate_process(qspi_Drv->process,qspi_Drv->wakeup_id,WAKEUP_FLAGS_QSPI_RX);
	}
	__enable_irq();
}

void HAL_QSPI_TxCpltCallback(QSPI_HandleTypeDef *hqspi)
{
QSPI_DriverStruct_t	*qspi_Drv;

	__disable_irq();
	qspi_Drv = qspi_irq_common(hqspi);
	if ( qspi_Drv != NULL )
	{
		qspi_Drv->status |= QSPI_WRITE_COMPLETE;
		activate_process(qspi_Drv->process,qspi_Drv->wakeup_id,WAKEUP_FLAGS_QSPI_RX);
	}
	__enable_irq();
}
#endif // #ifdef A_OS_QUADSPI_ENABLED

