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
 *  Created on: Nov 21, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef STM32H7xx_HAL_QSPI_H

#include "../qspi.h"
#include "w25qxx.h"
#include "w25qxx_defs.h"


ITCM_AREA_CODE static uint8_t w25qxx_seterror(W25Qxx_Drv_TypeDef	*w25qxx_Drv,uint8_t error)
{
	w25qxx_Drv->status &= ~QSPI_BUSY;
	w25qxx_Drv->status |= QSPI_ERROR;
	return error;
}

ITCM_AREA_CODE static uint8_t w25qxx_WaitForDriverFlag(W25Qxx_Drv_TypeDef *w25qxx_Drv , uint8_t flag , uint16_t timeout)
{
	if ( timeout == 0 )
		return 1;
	while( (w25qxx_Drv->status & flag) != flag )
	{
		task_delay(1);
		timeout--;
		if ( timeout == 0 )
			return W25Q_CHIP_ERR;
	}
	return 0;
}

ITCM_AREA_CODE static uint8_t send_qspi_com(W25Qxx_Drv_TypeDef	*w25qxx_Drv )
{
	if (HAL_QSPI_Command(w25qxx_Drv->qspi_bus, &w25qxx_Drv->com, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		w25qxx_Drv->status |= QSPI_ERROR;
		return W25Q_SPI_ERR;
	}
	return 0;
}


ITCM_AREA_CODE static uint8_t set_qspi_com(W25Qxx_Drv_TypeDef *w25qxx_Drv, uint8_t Instruction,uint32_t InstructionMode, uint32_t Address, uint32_t AddressMode, uint32_t NbData,uint32_t DataMode,uint32_t DummyCycles )
{
	w25qxx_Drv->com.InstructionMode = InstructionMode;
	w25qxx_Drv->com.Instruction = Instruction;
	w25qxx_Drv->com.AddressMode = AddressMode;
	w25qxx_Drv->com.Address = Address;
	w25qxx_Drv->com.DataMode = DataMode;
	w25qxx_Drv->com.NbData = NbData;
	w25qxx_Drv->com.DummyCycles = DummyCycles;
	return send_qspi_com(w25qxx_Drv);
}

ITCM_AREA_CODE uint32_t w25qxx_ReadStatusReg(uint32_t *w25qxx_Drv_e,uint8_t reg,uint16_t timeout)
{
W25Qxx_Drv_TypeDef *w25qxx_Drv = (W25Qxx_Drv_TypeDef *)w25qxx_Drv_e;
uint32_t			Instruction;

	switch(reg)
	{
	case 1 :  Instruction = W25Q_READ_SR1; break;
	case 2 :  Instruction = W25Q_READ_SR2; break;
	case 3 :  Instruction = W25Q_READ_SR3; break;
	default :  return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR); break;
	}

	if ( set_qspi_com(w25qxx_Drv, Instruction, QSPI_INSTRUCTION_1_LINE, 0, QSPI_ADDRESS_NONE, 1, QSPI_DATA_1_LINE,W25Q_DUMMY_0) )
		return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);
	if (HAL_QSPI_Receive(w25qxx_Drv->qspi_bus, &w25qxx_Drv->qspi_status_reg, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);
	return 0;
}

ITCM_AREA_CODE uint32_t w25qxx_WriteStatusReg(uint32_t *w25qxx_Drv_e,uint8_t reg,uint16_t timeout)
{
W25Qxx_Drv_TypeDef *w25qxx_Drv = (W25Qxx_Drv_TypeDef *)w25qxx_Drv_e;
uint32_t			Instruction;

	switch(reg)
	{
	case 1 :  Instruction = W25Q_WRITE_SR1; break;
	case 2 :  Instruction = W25Q_WRITE_SR2; break;
	case 3 :  Instruction = W25Q_WRITE_SR3; break;
	default :  return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR); break;
	}

	if ( set_qspi_com(w25qxx_Drv, Instruction, QSPI_INSTRUCTION_1_LINE, 0, QSPI_ADDRESS_NONE, 1, QSPI_DATA_1_LINE,W25Q_DUMMY_0) )
		return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);
	if (HAL_QSPI_Transmit_IT(w25qxx_Drv->qspi_bus, &w25qxx_Drv->qspi_status_reg) != HAL_OK)
		return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);
	return 0;
}

ITCM_AREA_CODE uint32_t w25qxx_ReadAllStatusRegs(uint32_t *w25qxx_Drv_e,uint16_t timeout)
{
W25Qxx_Drv_TypeDef *w25qxx_Drv = (W25Qxx_Drv_TypeDef *)w25qxx_Drv_e;
	if ( set_qspi_com(w25qxx_Drv, W25Q_READ_SR1, QSPI_INSTRUCTION_1_LINE, 0, QSPI_ADDRESS_NONE, 1, QSPI_DATA_1_LINE,W25Q_DUMMY_0) )
		return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);
	if (HAL_QSPI_Receive(w25qxx_Drv->qspi_bus, &w25qxx_Drv->qspi_status_reg1, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);

	if ( set_qspi_com(w25qxx_Drv, W25Q_READ_SR2, QSPI_INSTRUCTION_1_LINE, 0, QSPI_ADDRESS_NONE, 1, QSPI_DATA_1_LINE,W25Q_DUMMY_0) )
		return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);
	if (HAL_QSPI_Receive(w25qxx_Drv->qspi_bus, &w25qxx_Drv->qspi_status_reg2, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);

	if ( set_qspi_com(w25qxx_Drv, W25Q_READ_SR3, QSPI_INSTRUCTION_1_LINE, 0, QSPI_ADDRESS_NONE, 1, QSPI_DATA_1_LINE,W25Q_DUMMY_0) )
		return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);
	if (HAL_QSPI_Receive(w25qxx_Drv->qspi_bus, &w25qxx_Drv->qspi_status_reg3, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);

	return 0;
}

ITCM_AREA_CODE static uint8_t w25qxx_WaitIfFlashBusy(W25Qxx_Drv_TypeDef *w25qxx_Drv,uint16_t timeout)
{
	w25qxx_Drv->qspi_status_reg = W25Q_BUSY;
	while(( w25qxx_Drv->qspi_status_reg & W25Q_BUSY) == W25Q_BUSY )
	{
		task_delay(1);
		timeout--;
		if ( timeout == 0 )
			return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);
		w25qxx_ReadStatusReg((uint32_t *)w25qxx_Drv,1,W25Q_READ_TIMEOUT);
	}
	return 0;
}

ITCM_AREA_CODE static uint32_t w25qxx_read_cycle(W25Qxx_Drv_TypeDef	*w25qxx_Drv, uint8_t *data)
{
	if (( w25qxx_Drv->flags & QSPI_USES_DMA ) == QSPI_USES_DMA )
	{
		if ( HAL_QSPI_Receive_DMA(w25qxx_Drv->qspi_bus, data) != HAL_OK )
			return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);
	}
	else
	{
		if ( HAL_QSPI_Receive_IT(w25qxx_Drv->qspi_bus, data) != HAL_OK )
			return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);
	}
	if ( w25qxx_WaitForDriverFlag(w25qxx_Drv,QSPI_READ_COMPLETE, W25Q_READ_TIMEOUT) )
		return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);
	if ( w25qxx_WaitIfFlashBusy(w25qxx_Drv,W25Q_READ_TIMEOUT) )
		return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);

	return 0;
}

ITCM_AREA_CODE uint32_t w25qxx_read(uint32_t *w25qxx_Drv_e, uint32_t address,uint8_t *data,uint32_t data_len)
{
W25Qxx_Drv_TypeDef *w25qxx_Drv = (W25Qxx_Drv_TypeDef *)w25qxx_Drv_e;
uint32_t			Instruction;
uint32_t 			p_data_len;

	if (( w25qxx_Drv->status & QSPI_BUSY ) == QSPI_BUSY )
		return 1;
	w25qxx_Drv->status = QSPI_BUSY;
	Instruction = w25qxx_Drv->FlashSize > 128U ? W25Q_FAST_READ_QUAD_IO_4B : W25Q_FAST_READ_QUAD_IO;

	if ( (address & 0xff ) != 0 )
	{
		if ( ((address & 0xff ) + data_len) <=  W25Q_MEM_PAGE_SIZE)
		{
			if ( set_qspi_com(w25qxx_Drv, Instruction, QSPI_INSTRUCTION_1_LINE, address, QSPI_ADDRESS_4_LINES, data_len, QSPI_DATA_4_LINES,W25Q_DUMMY_6) )
				return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);
			if ( w25qxx_read_cycle(w25qxx_Drv,data) )
				return 1;
			data_len = 0;
		}
		else
		{
			p_data_len = W25Q_MEM_PAGE_SIZE - (address & 0xff );
			if ( set_qspi_com(w25qxx_Drv, Instruction, QSPI_INSTRUCTION_1_LINE, address, QSPI_ADDRESS_4_LINES, p_data_len, QSPI_DATA_4_LINES,W25Q_DUMMY_6) )
				return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);
			if ( w25qxx_read_cycle(w25qxx_Drv,data) )
				return 1;
			data_len -= p_data_len;
			address &= 0xffffff00;
			address += 0x100;
			data += p_data_len;

		}
	}

	while ( data_len  > W25Q_MEM_PAGE_SIZE)
	{
		if ( w25qxx_WaitIfFlashBusy(w25qxx_Drv,W25Q_READ_TIMEOUT) )
			return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);

		if ( set_qspi_com(w25qxx_Drv, Instruction, QSPI_INSTRUCTION_1_LINE, address, QSPI_ADDRESS_4_LINES, W25Q_MEM_PAGE_SIZE, QSPI_DATA_4_LINES,W25Q_DUMMY_6) )
			return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);

		if ( w25qxx_read_cycle(w25qxx_Drv,data) )
			return 1;

		data_len -= W25Q_MEM_PAGE_SIZE;
		address += W25Q_MEM_PAGE_SIZE;
		data += W25Q_MEM_PAGE_SIZE;
	}
	if ( data_len )
	{
		if ( w25qxx_WaitIfFlashBusy(w25qxx_Drv,W25Q_READ_TIMEOUT) )
			return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);

		if ( set_qspi_com(w25qxx_Drv, Instruction, QSPI_INSTRUCTION_1_LINE, address, QSPI_ADDRESS_4_LINES, W25Q_MEM_PAGE_SIZE, QSPI_DATA_4_LINES,W25Q_DUMMY_6) )
			return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);

		if ( w25qxx_read_cycle(w25qxx_Drv,data) )
			return 1;
	}

	w25qxx_Drv->status &= ~QSPI_BUSY;

	return 0;

}

uint8_t w25qxx_WriteEnable(W25Qxx_Drv_TypeDef *w25qxx_Drv,uint8_t enable)
{
uint32_t	Instruction;

	if ( w25qxx_WaitIfFlashBusy(w25qxx_Drv,W25Q_WRITE_TIMEOUT) )
		return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);
	Instruction = enable ? W25Q_WRITE_ENABLE : W25Q_WRITE_DISABLE;

	if ( set_qspi_com(w25qxx_Drv, Instruction, QSPI_INSTRUCTION_1_LINE, 0, QSPI_ADDRESS_NONE, 0, QSPI_DATA_NONE,W25Q_DUMMY_0) )
		return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);

	w25qxx_Drv->status |= QSPI_WEL;

	if ( w25qxx_WaitIfFlashBusy(w25qxx_Drv,W25Q_WRITE_TIMEOUT) )
		return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);

	return 0;
}

ITCM_AREA_CODE static uint32_t w25qxx_write_cycle(W25Qxx_Drv_TypeDef	*w25qxx_Drv, uint8_t *data)
{
	if (( w25qxx_Drv->flags & QSPI_USES_DMA ) == QSPI_USES_DMA )
	{
		if (HAL_QSPI_Transmit_DMA(w25qxx_Drv->qspi_bus, data) != HAL_OK)
			return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);
	}
	else
	{
		if (HAL_QSPI_Transmit_IT(w25qxx_Drv->qspi_bus, data) != HAL_OK)
			return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);
	}

	if ( w25qxx_WaitForDriverFlag(w25qxx_Drv,QSPI_WRITE_COMPLETE,W25Q_WRITE_TIMEOUT) )
		return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);

	if ( w25qxx_WaitIfFlashBusy(w25qxx_Drv,W25Q_WRITE_TIMEOUT) )
		return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);
	return 0;
}

ITCM_AREA_CODE uint32_t w25qxx_write(uint32_t *w25qxx_Drv_e, uint32_t address,uint8_t *data,uint32_t data_len)
{
W25Qxx_Drv_TypeDef *w25qxx_Drv = (W25Qxx_Drv_TypeDef *)w25qxx_Drv_e;
uint32_t			Instruction;
uint32_t 			p_data_len;

	if (( w25qxx_Drv->status & QSPI_BUSY ) == QSPI_BUSY )
		return 1;
	w25qxx_Drv->status = QSPI_BUSY;

	Instruction = w25qxx_Drv->FlashSize > 128U ? W25Q_PAGE_PROGRAM_QUAD_INP_4B : W25Q_PAGE_PROGRAM_QUAD_INP;

	w25qxx_Drv->status &= ~QSPI_WRITE_COMPLETE;

	if ( (address & 0xff ) != 0 )
	{
		if (w25qxx_WriteEnable(w25qxx_Drv,W25Q_ENABLE_WRITE) != W25Q_OK)
			return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);

		if ( ((address & 0xff ) + data_len) <=  W25Q_MEM_PAGE_SIZE)
		{
			if ( set_qspi_com(w25qxx_Drv, Instruction, QSPI_INSTRUCTION_1_LINE, address, QSPI_ADDRESS_1_LINE, data_len, QSPI_DATA_4_LINES,W25Q_DUMMY_0) )
				return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);
			if ( w25qxx_write_cycle(w25qxx_Drv, data))
				return 1;
			data_len = 0;
		}
		else
		{
			p_data_len = W25Q_MEM_PAGE_SIZE - (address & 0xff );

			if ( set_qspi_com(w25qxx_Drv, Instruction, QSPI_INSTRUCTION_1_LINE, address, QSPI_ADDRESS_1_LINE, p_data_len, QSPI_DATA_4_LINES,W25Q_DUMMY_0) )
				return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);
			if ( w25qxx_write_cycle(w25qxx_Drv, data))
				return 1;

			data_len -= p_data_len;
			address &= 0xffffff00;
			address += 0x100;
			data += p_data_len;
		}
		if (w25qxx_WriteEnable(w25qxx_Drv,W25Q_DISABLE_WRITE) != W25Q_OK)
			return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);
	}

	while ( data_len >= W25Q_MEM_PAGE_SIZE)
	{
		if (w25qxx_WriteEnable(w25qxx_Drv,W25Q_ENABLE_WRITE) != W25Q_OK)
			return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);

		if ( set_qspi_com(w25qxx_Drv, Instruction, QSPI_INSTRUCTION_1_LINE, address, QSPI_ADDRESS_1_LINE, W25Q_MEM_PAGE_SIZE, QSPI_DATA_4_LINES,W25Q_DUMMY_0) )
			return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);

		if ( w25qxx_write_cycle(w25qxx_Drv, data))
			return 1;

		if (w25qxx_WriteEnable(w25qxx_Drv,W25Q_DISABLE_WRITE) != W25Q_OK)
			return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);

		data_len -= W25Q_MEM_PAGE_SIZE;
		address += W25Q_MEM_PAGE_SIZE;
		data += W25Q_MEM_PAGE_SIZE;
	}
	if ( data_len )
	{
		if (w25qxx_WriteEnable(w25qxx_Drv,W25Q_ENABLE_WRITE) != W25Q_OK)
			return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);

		if ( set_qspi_com(w25qxx_Drv, Instruction, QSPI_INSTRUCTION_1_LINE, address, QSPI_ADDRESS_1_LINE, data_len, QSPI_DATA_4_LINES,W25Q_DUMMY_0) )
			return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);

		if ( w25qxx_write_cycle(w25qxx_Drv, data))
			return 1;

		if (w25qxx_WriteEnable(w25qxx_Drv,W25Q_DISABLE_WRITE) != W25Q_OK)
			return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);
	}

	w25qxx_Drv->status &= ~QSPI_BUSY;

	return 0;
}

ITCM_AREA_CODE uint32_t w25qxx_erasesectors(uint32_t *w25qxx_Drv_e, uint32_t start_sector, uint32_t number_of_sectors)
{
W25Qxx_Drv_TypeDef *w25qxx_Drv = (W25Qxx_Drv_TypeDef *)w25qxx_Drv_e;
uint32_t Address, Instruction , i;

	if ( start_sector+number_of_sectors >= W25Q_BLOCK_COUNT)
		return W25Q_PARAM_ERR;
	if (( w25qxx_Drv->status & QSPI_BUSY ) == QSPI_BUSY )
		return 1;

	if (w25qxx_WaitIfFlashBusy(w25qxx_Drv, W25Q_ERASESECTORS_TIMEOUT))
		return w25qxx_seterror(w25qxx_Drv, W25Q_SPI_ERR);

    w25qxx_Drv->status = QSPI_BUSY;

    Instruction = W25Q_SECTOR_ERASE;

	for(i=0;i<number_of_sectors;i++)
	{
	    Address = start_sector * W25Q_MEM_SECTOR_SIZE;
		if (w25qxx_WriteEnable(w25qxx_Drv, 1) != W25Q_OK)
			return w25qxx_seterror(w25qxx_Drv, W25Q_SPI_ERR);
		if (set_qspi_com(w25qxx_Drv, Instruction, QSPI_INSTRUCTION_1_LINE, Address, QSPI_ADDRESS_1_LINE, 0, QSPI_DATA_NONE, W25Q_DUMMY_0))
			return w25qxx_seterror(w25qxx_Drv, W25Q_SPI_ERR);
		if (w25qxx_WaitIfFlashBusy(w25qxx_Drv, W25Q_ERASESECTORS_TIMEOUT))
			return w25qxx_seterror(w25qxx_Drv, W25Q_SPI_ERR);
		if (w25qxx_WriteEnable(w25qxx_Drv, 0) != W25Q_OK)
			return w25qxx_seterror(w25qxx_Drv, W25Q_SPI_ERR);
		start_sector++;
	}

    w25qxx_Drv->status &= ~QSPI_BUSY;

    return 0;
}

ITCM_AREA_CODE uint32_t w25qxx_eraseblocks(uint32_t *w25qxx_Drv_e, uint32_t start_block, uint32_t number_of_blocks)
{
uint32_t			Address,Instruction,i;

	if ( start_block+number_of_blocks >= W25Q_BLOCK_COUNT)
		return W25Q_PARAM_ERR;
	W25Qxx_Drv_TypeDef *w25qxx_Drv = (W25Qxx_Drv_TypeDef *)w25qxx_Drv_e;
	if (( w25qxx_Drv->status & QSPI_BUSY ) == QSPI_BUSY )
		return 1;
	if ( w25qxx_WaitIfFlashBusy(w25qxx_Drv,W25Q_ERASEBLOCKS_TIMEOUT) )
		return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);

	w25qxx_Drv->status = QSPI_BUSY;

	Instruction = w25qxx_Drv->FlashSize > 128U ? W25Q_64KB_BLOCK_ERASE_4B : W25Q_32KB_BLOCK_ERASE;

	for(i=start_block;i<start_block+number_of_blocks;i++)
	{
		Address = (i * W25Q_MEM_SBLOCK_SIZE * 1024U );
		if (w25qxx_WriteEnable(w25qxx_Drv,1) != W25Q_OK)
			return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);
		if ( set_qspi_com(w25qxx_Drv, Instruction    , QSPI_INSTRUCTION_1_LINE, Address, QSPI_ADDRESS_1_LINE, 0,QSPI_DATA_NONE,W25Q_DUMMY_0) )
			return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);
		if ( w25qxx_WaitIfFlashBusy(w25qxx_Drv,W25Q_ERASEBLOCKS_TIMEOUT) )
			return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);
		if (w25qxx_WriteEnable(w25qxx_Drv,0) != W25Q_OK)
			return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);
	}

	w25qxx_Drv->status &= ~QSPI_BUSY;

	return 0;
}

ITCM_AREA_CODE uint32_t w25qxx_erasechip(uint32_t *w25qxx_Drv_e)
{
W25Qxx_Drv_TypeDef *w25qxx_Drv = (W25Qxx_Drv_TypeDef *)w25qxx_Drv_e;
	w25qxx_Drv->status = 0;

	if (( w25qxx_Drv->status & QSPI_BUSY ) == QSPI_BUSY )
		return 1;
	w25qxx_Drv->status = QSPI_BUSY;

	if (w25qxx_WriteEnable(w25qxx_Drv,1) != W25Q_OK)
		return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);

	if ( set_qspi_com(w25qxx_Drv, W25Q_CHIP_ERASE, QSPI_INSTRUCTION_1_LINE, 0, QSPI_ADDRESS_NONE, 0, QSPI_DATA_NONE,W25Q_DUMMY_0) )
		return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);

	if ( w25qxx_WaitIfFlashBusy(w25qxx_Drv,W25Q_ERASECHIP_TIMEOUT) )
		return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);

	w25qxx_Drv->status &= ~QSPI_BUSY;
	return 0;
}

ITCM_AREA_CODE uint32_t w25qxx_GetID(uint32_t *w25qxx_Drv_e, uint8_t *data)
{
W25Qxx_Drv_TypeDef *w25qxx_Drv = (W25Qxx_Drv_TypeDef *)w25qxx_Drv_e;

	if ( set_qspi_com(w25qxx_Drv, W25Q_DEVID, QSPI_INSTRUCTION_1_LINE, 0,  QSPI_ADDRESS_1_LINE, 1, QSPI_DATA_1_LINE,W25Q_DUMMY_0) )
		return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);

	if ( HAL_QSPI_Receive_IT(w25qxx_Drv->qspi_bus, data) != HAL_OK )
		return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);
	if ( w25qxx_WaitForDriverFlag(w25qxx_Drv,QSPI_READ_COMPLETE, W25Q_READ_TIMEOUT) )
		return w25qxx_seterror(w25qxx_Drv,W25Q_SPI_ERR);
	w25qxx_Drv->qspi_id = *data;
	return 0;
}



#endif // #ifdef QSPI_ENABLED
