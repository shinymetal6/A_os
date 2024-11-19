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
 * w25qxx.c
 *
 *  Created on: Nov 18, 2024
 *      Author: fil
 */
#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"
#include "../../../kernel/kernel_opt.h"

#include "w25qxx.h"
#include "w25q_defs.h"

#include <string.h>

extern	ExtFlash_DriverStruct_t		ExtFlashDriverStruct[MAX_EXTMEM_DRIVERS];
extern	uint8_t						last_extflash_used_handle,extflash_driver_request;

ITCM_AREA_CODE static void set_com(QSPI_CommandTypeDef *com , uint8_t instruction,uint32_t instruction_mode, uint32_t address, uint32_t address_mode, uint32_t address_size, uint32_t nbdata,uint32_t datamode)
{
	com->InstructionMode = instruction_mode; 	// Instruction
	com->Instruction = instruction;	 				// Command
	com->AddressMode = address_mode;
	com->AddressSize = address_size;
	com->Address = address;
	com->AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	com->AlternateBytes = QSPI_ALTERNATE_BYTES_NONE;
	com->AlternateBytesSize = QSPI_ALTERNATE_BYTES_NONE;
	com->DummyCycles = 0;
	com->DataMode = datamode;
	com->NbData = nbdata;
	com->DdrMode = QSPI_DDR_MODE_DISABLE;
	com->DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	com->SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
}

ITCM_AREA_CODE static uint8_t w25qxx_ReadStatusReg(W25Qxx_Drv_TypeDef *w25qxx_Drv,uint8_t *reg_data, uint8_t reg_num)
{
QSPI_CommandTypeDef com;
uint32_t	instruction;

	if (reg_num == 1)
		instruction = W25Q_READ_SR1;
	else if (reg_num == 2)
		instruction = W25Q_READ_SR2;
	else if (reg_num == 3)
		instruction = W25Q_READ_SR3;
	else
		return W25Q_PARAM_ERR;
	set_com(&com , instruction,QSPI_INSTRUCTION_1_LINE, 0, QSPI_ADDRESS_NONE, QSPI_ADDRESS_NONE, 1,QSPI_DATA_1_LINE);

	if (HAL_QSPI_Command(w25qxx_Drv->qspi_bus, &com, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return W25Q_SPI_ERR;
	if (HAL_QSPI_Receive(w25qxx_Drv->qspi_bus, reg_data, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return W25Q_SPI_ERR;
	return W25Q_OK;
}


uint8_t w25qxx_WriteEnable(W25Qxx_Drv_TypeDef *w25qxx_Drv,uint8_t enable)
{
QSPI_CommandTypeDef com;
uint32_t	instruction;

	instruction = enable ? W25Q_WRITE_ENABLE : W25Q_WRITE_DISABLE;
	set_com(&com , instruction,QSPI_INSTRUCTION_1_LINE, 0, QSPI_ADDRESS_NONE, QSPI_ADDRESS_NONE, 0,QSPI_DATA_NONE);

	if (HAL_QSPI_Command(w25qxx_Drv->qspi_bus, &com, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return W25Q_SPI_ERR;
	task_delay(1); // Give a little time to sleep

	w25qxx_Drv->status |= QSPI_WEL;

	return W25Q_OK;
}

ITCM_AREA_CODE static uint8_t w25qxx_IsBusy(W25Qxx_Drv_TypeDef *w25qxx_Drv)
{
uint8_t sr = 0;

	if (w25qxx_ReadStatusReg( w25qxx_Drv, &sr, 1) != W25Q_OK)
		return W25Q_CHIP_ERR;
	return sr & 0x01;
}

ITCM_AREA_CODE uint32_t w25qxx_read(uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len)
{
W25Qxx_Drv_TypeDef	*w25qxx_Drv;
QSPI_CommandTypeDef com;
uint32_t			Instruction,AddressSize;

	w25qxx_Drv = (W25Qxx_Drv_TypeDef *)ExtFlashDriverStruct[handle].driver_private_data;
	w25qxx_Drv->status = 0;

	if (data_len > 256 || data_len == 0)
	{
		w25qxx_Drv->status |= QSPI_ERROR;
		return W25Q_PARAM_ERR;
	}

	#if MEM_FLASH_SIZE > 128U
		Instruction = W25Q_FAST_READ_QUAD_IO_4B;
		com.AddressSize = QSPI_ADDRESS_32_BITS;
	#else
		Instruction = W25Q_FAST_READ_QUAD_IO;
		AddressSize = QSPI_ADDRESS_24_BITS;
	#endif
		set_com(&com, Instruction, QSPI_INSTRUCTION_1_LINE, address, QSPI_ADDRESS_4_LINES, AddressSize, data_len, QSPI_DATA_4_LINES);
		com.DummyCycles = 6;
		if (HAL_QSPI_Command(w25qxx_Drv->qspi_bus, &com, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		{
			w25qxx_Drv->status |= QSPI_ERROR;
			return W25Q_SPI_ERR;
		}
		if (( w25qxx_Drv->flags & QSPI_USES_DMA ) == QSPI_USES_DMA )
		{
			w25qxx_Drv->status &= ~QSPI_READ_DMA_COMPLETE;
			if ( HAL_QSPI_Receive_DMA(w25qxx_Drv->qspi_bus, data) != HAL_OK )
			{
				w25qxx_Drv->status |= QSPI_ERROR;
				return W25Q_SPI_ERR;
			}
		}
		else
		{
			w25qxx_Drv->status &= ~QSPI_READ_COMPLETE;
			if ( HAL_QSPI_Receive_IT(w25qxx_Drv->qspi_bus, data) != HAL_OK )
			{
				w25qxx_Drv->status |= QSPI_ERROR;
				return W25Q_SPI_ERR;
			}
		}
		com.DummyCycles = 0;
		return 0;
}

ITCM_AREA_CODE uint32_t w25qxx_write(uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len)
{
W25Qxx_Drv_TypeDef	*w25qxx_Drv = (W25Qxx_Drv_TypeDef *)ExtFlashDriverStruct[handle].driver_private_data;
QSPI_CommandTypeDef com;
uint32_t			Instruction,AddressSize;
uint8_t				state;

		if (address > (MEM_FLASH_SIZE_FULL-(1024U*8)) )
			return W25Q_PARAM_ERR;

		if (data_len > MEM_PAGE_SIZE || data_len == 0)
			return W25Q_PARAM_ERR;

		while (w25qxx_IsBusy(w25qxx_Drv) == W25Q_BUSY)
			task_delay(1);

		state = w25qxx_WriteEnable(w25qxx_Drv,1);
		if (state != W25Q_OK)
			return state;

	#if MEM_FLASH_SIZE > 128U
		Instruction = W25Q_PAGE_PROGRAM_QUAD_INP_4B;	 // Command
		AddressSize = QSPI_ADDRESS_32_BITS;
	#else
		Instruction = W25Q_PAGE_PROGRAM_QUAD_INP;	 // Command
		AddressSize = QSPI_ADDRESS_24_BITS;
	#endif

		set_com(&com, Instruction, QSPI_INSTRUCTION_1_LINE, address, QSPI_ADDRESS_1_LINE, AddressSize, data_len, QSPI_DATA_4_LINES);
		if (HAL_QSPI_Command(w25qxx_Drv->qspi_bus, &com, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		{
			w25qxx_Drv->status |= QSPI_ERROR;
			return W25Q_SPI_ERR;
		}

		if (HAL_QSPI_Transmit(w25qxx_Drv->qspi_bus, data, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		{
			w25qxx_Drv->status |= QSPI_ERROR;
			return W25Q_SPI_ERR;
		}

		w25qxx_Drv->status &= ~QSPI_WRITE_COMPLETE;
		while (w25qxx_IsBusy(w25qxx_Drv) == W25Q_BUSY)
			task_delay(1);

		return W25Q_OK;
}

ITCM_AREA_CODE uint32_t w25qxx_eraseblocks(uint8_t handle, uint32_t start_block, uint32_t number_of_blocks)
{
W25Qxx_Drv_TypeDef	*w25qxx_Drv = (W25Qxx_Drv_TypeDef *)ExtFlashDriverStruct[handle].driver_private_data;
w25qxx_Drv->status = 0;
QSPI_CommandTypeDef com;
uint32_t			Instruction,AddressSize;
uint32_t			Address;
uint8_t				state;

	if ( number_of_blocks >= BLOCK_COUNT)
		return W25Q_PARAM_ERR;

	while (w25qxx_IsBusy(w25qxx_Drv) == W25Q_BUSY)
		task_delay(1);

	state = w25qxx_WriteEnable(w25qxx_Drv,1);
	if (state != W25Q_OK)
		return state;

#if MEM_FLASH_SIZE > 128U
	rawAddr = BlockNumber * MEM_BLOCK_SIZE * 1024U;
	Instruction = W25Q_64KB_BLOCK_ERASE_4B;	 // Command
	AddressSize = QSPI_ADDRESS_32_BITS;
#else
	Address = (start_block * MEM_SBLOCK_SIZE * 1024U ) + (number_of_blocks * MEM_SBLOCK_SIZE * 1024U);
	Instruction = W25Q_32KB_BLOCK_ERASE;	 // Command
	AddressSize = QSPI_ADDRESS_24_BITS;
#endif

	set_com(&com, Instruction, QSPI_INSTRUCTION_1_LINE, Address, QSPI_ADDRESS_1_LINE, AddressSize, 0, QSPI_DATA_NONE);
	if (HAL_QSPI_Command(w25qxx_Drv->qspi_bus, &com, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return W25Q_SPI_ERR;

	while (w25qxx_IsBusy(w25qxx_Drv) == W25Q_BUSY)
		task_delay(1);

	return W25Q_OK;
}

ITCM_AREA_CODE uint32_t w25qxx_erasechip(uint8_t handle)
{
W25Qxx_Drv_TypeDef	*w25qxx_Drv = (W25Qxx_Drv_TypeDef *)ExtFlashDriverStruct[handle].driver_private_data;
w25qxx_Drv->status = 0;
QSPI_CommandTypeDef com;
uint8_t				state;

	while (w25qxx_IsBusy(w25qxx_Drv) == W25Q_BUSY)
		task_delay(1);

	state = w25qxx_WriteEnable(w25qxx_Drv,1);
	if (state != W25Q_OK)
		return state;

	set_com(&com, W25Q_CHIP_ERASE, QSPI_INSTRUCTION_1_LINE, 0, QSPI_ADDRESS_NONE, QSPI_ADDRESS_NONE, 0, QSPI_DATA_NONE);

	if (HAL_QSPI_Command(w25qxx_Drv->qspi_bus, &com, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return W25Q_SPI_ERR;

	while (w25qxx_IsBusy(w25qxx_Drv) == W25Q_BUSY)
		task_delay(1);

	return W25Q_OK;
}

ITCM_AREA_CODE uint32_t	w25qxx_register(W25Qxx_Drv_TypeDef *driver_private_data)
{
W25Qxx_Drv_TypeDef	*w25qxx_Drv;
	if ( ExtFlashDriverStruct[last_extflash_used_handle].process == 0 )
	{
		if ( driver_private_data->wakeup_id == 0 )
			return DRIVER_REQUEST_FAILED;
		ExtFlashDriverStruct[last_extflash_used_handle].process = get_current_process();
		ExtFlashDriverStruct[last_extflash_used_handle].driver_private_data = (uint32_t *)driver_private_data;
		ExtFlashDriverStruct[last_extflash_used_handle].read = w25qxx_read;
		ExtFlashDriverStruct[last_extflash_used_handle].write = w25qxx_write;
		ExtFlashDriverStruct[last_extflash_used_handle].erase_blocks = w25qxx_eraseblocks;
		ExtFlashDriverStruct[last_extflash_used_handle].erase_chip = w25qxx_erasechip;

		w25qxx_Drv = (W25Qxx_Drv_TypeDef *)ExtFlashDriverStruct[last_extflash_used_handle].driver_private_data;

		if ( w25qxx_Drv->qspi_bus  ->hmdma == NULL )
		{
			/* disable dma if it's not configured in hw */
			w25qxx_Drv->flags &= ~QSPI_USES_DMA;
		}
		ExtFlashDriverStruct[last_extflash_used_handle].status = DRIVER_STATUS_REQUESTED;
		last_extflash_used_handle++;
		extflash_driver_request++;
		return last_extflash_used_handle-1;
	}
	return DRIVER_REQUEST_FAILED;
}

/***** Interrupts ********/
ITCM_AREA_CODE static uint8_t find_handle_from_qspi(QSPI_HandleTypeDef *qspi)
{
uint8_t	i;
W25Qxx_Drv_TypeDef	*w25qxx_Drv;

	for(i=0;i<MAX_EXTMEM_DRIVERS;i++)
	{
		w25qxx_Drv = (W25Qxx_Drv_TypeDef *)ExtFlashDriverStruct[i].driver_private_data;
		if ( qspi == w25qxx_Drv->qspi_bus)
			return i;
	}
	return 255;
}
void HAL_QSPI_RxCpltCallback(QSPI_HandleTypeDef *hqspi)
{
uint8_t	handle;
W25Qxx_Drv_TypeDef	*w25qxx_Drv;

	__disable_irq();
	if ( (handle = find_handle_from_qspi(hqspi)) != 255)
	{
		w25qxx_Drv = (W25Qxx_Drv_TypeDef *)ExtFlashDriverStruct[handle].driver_private_data;
		w25qxx_Drv->status |= QSPI_READ_COMPLETE;
		activate_process(ExtFlashDriverStruct[handle].process,w25qxx_Drv->wakeup_id,WAKEUP_FLAGS_QSPI_RX);
	}
	__enable_irq();
}

void HAL_QSPI_TxCpltCallback(QSPI_HandleTypeDef *hqspi)
{
uint8_t	handle;
W25Qxx_Drv_TypeDef	*w25qxx_Drv;

	__disable_irq();
	if ( (handle = find_handle_from_qspi(hqspi)) != 255)
	{
		w25qxx_Drv = (W25Qxx_Drv_TypeDef *)ExtFlashDriverStruct[handle].driver_private_data;
		w25qxx_Drv->status |= QSPI_WRITE_COMPLETE;
		activate_process(ExtFlashDriverStruct[handle].process,w25qxx_Drv->wakeup_id,WAKEUP_FLAGS_QSPI_TX);
	}
	__enable_irq();
}



