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
 * qspi_w25q128.c
 *
 *  Created on: Nov 17, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"

#ifdef STM32H7xx_HAL_QSPI_H

#include "qspi_w25q128.h"
#include "string.h"

extern	QSPI_HandleTypeDef hqspi;
uint32_t W25Q128JV_ReadID(uint32_t *qspi_Drv_in)
{
    uint8_t buffer[3];
    QSPI_DriverStruct_t *qspi_Drv = (QSPI_DriverStruct_t *)qspi_Drv_in;

    bzero(&qspi_Drv->cmd,sizeof(QSPI_CommandTypeDef));
    qspi_Drv->cmd.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    qspi_Drv->cmd.Instruction       = W25Q128JV_CMD_READ_ID;
    qspi_Drv->cmd.AddressMode       = QSPI_ADDRESS_NONE;
    qspi_Drv->cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    qspi_Drv->cmd.DataMode          = QSPI_DATA_1_LINE;
    qspi_Drv->cmd.DummyCycles       = 0;
    qspi_Drv->cmd.NbData            = 3;
    qspi_Drv->cmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
    qspi_Drv->cmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    qspi_Drv->cmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(qspi_Drv->qspi, &qspi_Drv->cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return 1;
    }

    if (HAL_QSPI_Receive(&hqspi, buffer, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return 1;
    }

    qspi_Drv->man_id = buffer[0];
    qspi_Drv->dev_id = buffer[1];
    qspi_Drv->capacity = buffer[2];
    return 0;
}

static uint8_t W25Q128JV_ReadStatus(QSPI_DriverStruct_t *qspi_Drv)
{
    bzero(&qspi_Drv->cmd,sizeof(QSPI_CommandTypeDef));
    uint8_t status;

    qspi_Drv->cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    qspi_Drv->cmd.Instruction     = W25Q128JV_CMD_READ_STATUS1;
    qspi_Drv->cmd.AddressMode     = QSPI_ADDRESS_NONE;
    qspi_Drv->cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    qspi_Drv->cmd.DataMode        = QSPI_DATA_1_LINE;
    qspi_Drv->cmd.DummyCycles     = 0;
    qspi_Drv->cmd.NbData          = 1;
    qspi_Drv->cmd.DdrMode         = QSPI_DDR_MODE_DISABLE;
    qspi_Drv->cmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    qspi_Drv->cmd.SIOOMode        = QSPI_SIOO_INST_EVERY_CMD;

    HAL_QSPI_Command(qspi_Drv->qspi, &qspi_Drv->cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    HAL_QSPI_Receive(qspi_Drv->qspi, &status, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);

    return status;
}

static uint32_t W25Q128JV_WriteEnable(QSPI_DriverStruct_t *qspi_Drv)
{
    bzero(&qspi_Drv->cmd,sizeof(QSPI_CommandTypeDef));

    qspi_Drv->cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    qspi_Drv->cmd.Instruction     = W25Q128JV_CMD_WRITE_ENABLE;
    qspi_Drv->cmd.AddressMode     = QSPI_ADDRESS_NONE;
    qspi_Drv->cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    qspi_Drv->cmd.DataMode        = QSPI_DATA_NONE;
    qspi_Drv->cmd.DummyCycles     = 0;
    qspi_Drv->cmd.NbData          = 0;
    qspi_Drv->cmd.DdrMode         = QSPI_DDR_MODE_DISABLE;
    qspi_Drv->cmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    qspi_Drv->cmd.SIOOMode        = QSPI_SIOO_INST_EVERY_CMD;

    return HAL_QSPI_Command(qspi_Drv->qspi, &qspi_Drv->cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
}

static uint32_t W25Q128JV_WaitForReady(QSPI_DriverStruct_t *qspi_Drv,uint32_t timeout)
{
    uint32_t start = HAL_GetTick();
    while (W25Q128JV_ReadStatus(qspi_Drv) & W25Q128JV_BUSY_MASK) {
        if ((HAL_GetTick() - start) > timeout)
        {
            return 1;
        }
        task_delay(1);
    }
    return 0;
}

uint32_t W25Q128JV_EraseSector(uint32_t *qspi_Drv_in,uint32_t sector_addr)
{
    if (sector_addr >= W25Q128JV_CAPACITY_BYTES || sector_addr % W25Q128JV_SECTOR_SIZE != 0)
        return 1;
    QSPI_DriverStruct_t *qspi_Drv = (QSPI_DriverStruct_t *)qspi_Drv_in;

    if (W25Q128JV_WaitForReady(qspi_Drv,5000) != 0)
    	return 1;
    if (W25Q128JV_WriteEnable(qspi_Drv) != 0)
    	return 1;

    bzero(&qspi_Drv->cmd,sizeof(QSPI_CommandTypeDef));
    qspi_Drv-> cmd.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    qspi_Drv->cmd.Instruction       = W25Q128JV_CMD_SECTOR_ERASE;
    qspi_Drv->cmd.AddressMode       = QSPI_ADDRESS_1_LINE;
    qspi_Drv->cmd.AddressSize       = QSPI_ADDRESS_24_BITS;
    qspi_Drv->cmd.Address           = sector_addr;
    qspi_Drv->cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    qspi_Drv->cmd.DataMode          = QSPI_DATA_NONE;
    qspi_Drv->cmd.DummyCycles       = 0;
    qspi_Drv->cmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
    qspi_Drv->cmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    qspi_Drv->cmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    HAL_StatusTypeDef status = HAL_QSPI_Command(&hqspi, &qspi_Drv->cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    return status == HAL_OK ? W25Q128JV_WaitForReady(qspi_Drv,5000) : status;
}

uint32_t W25Q128JV_PagesProgram(uint32_t *qspi_Drv_in,uint32_t address, uint8_t* data, uint32_t len)
{
size_t i_len = len;
uint32_t i_address = address;
uint8_t *i_data = data;

    if (address + len > W25Q128JV_CAPACITY_BYTES)
        return 1;
    QSPI_DriverStruct_t *qspi_Drv = (QSPI_DriverStruct_t *)qspi_Drv_in;
    if (W25Q128JV_WaitForReady(qspi_Drv,5000) != HAL_OK)
    	return 1;

    while( i_len )
    {
        if (W25Q128JV_WaitForReady(qspi_Drv,5000) != HAL_OK)
        	return 1;
        if (W25Q128JV_WriteEnable(qspi_Drv) != HAL_OK)
        	return 1;
        bzero(&qspi_Drv->cmd,sizeof(QSPI_CommandTypeDef));
        qspi_Drv->cmd.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
        qspi_Drv->cmd.Instruction       = W25Q128JV_CMD_PAGE_PROGRAM;
        qspi_Drv->cmd.AddressMode       = QSPI_ADDRESS_1_LINE;
        qspi_Drv->cmd.AddressSize       = QSPI_ADDRESS_24_BITS;
        qspi_Drv->cmd.Address           = i_address;
        qspi_Drv->cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
        qspi_Drv->cmd.DataMode          = QSPI_DATA_1_LINE;
        if ( i_len > W25Q128JV_PAGE_SIZE)
        	qspi_Drv->cmd.NbData            = W25Q128JV_PAGE_SIZE ;
        else
        	qspi_Drv->cmd.NbData            = i_len ;
        qspi_Drv->cmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
        qspi_Drv->cmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
        qspi_Drv->cmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
        qspi_Drv->cmd.DummyCycles 	  = W25Q_DUMMY_0;

        if( HAL_QSPI_Command(&hqspi, &qspi_Drv->cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        	return 1;
        qspi_Drv->status &= ~QSPI_DMA_WRITE_COMPLETE;
        qspi_Drv->wait_dma_timeout = 100;
        if( HAL_QSPI_Transmit_DMA(&hqspi, i_data) != HAL_OK)
        	return 1;
        while((qspi_Drv->status & QSPI_DMA_WRITE_COMPLETE) == 0)
        {
        	task_delay(1);
        	qspi_Drv->wait_dma_timeout--;
        	if ( qspi_Drv->wait_dma_timeout == 0 )
        		return 1;
        }

        i_address += W25Q128JV_PAGE_SIZE;
        i_data += W25Q128JV_PAGE_SIZE;
        if (i_len >= W25Q128JV_PAGE_SIZE)
        	i_len -= W25Q128JV_PAGE_SIZE;
        if ( i_len == 0 )
        	return 0;
    }
	return 0;
}

uint32_t W25Q128JV_FastReadQuad(uint32_t *qspi_Drv_in,uint32_t address, uint8_t* data, uint32_t len)
{
    if (address + len > W25Q128JV_CAPACITY_BYTES)
    	return 1;
    QSPI_DriverStruct_t *qspi_Drv = (QSPI_DriverStruct_t *)qspi_Drv_in;
    bzero(&qspi_Drv->cmd,sizeof(QSPI_CommandTypeDef));
    qspi_Drv->cmd.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    qspi_Drv->cmd.Instruction       = W25Q128JV_CMD_FAST_READ_QUAD;
    qspi_Drv->cmd.AddressMode       = QSPI_ADDRESS_4_LINES;
    qspi_Drv->cmd.AddressSize       = QSPI_ADDRESS_24_BITS;
    qspi_Drv->cmd.Address           = address;
    qspi_Drv->cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    qspi_Drv->cmd.DataMode          = QSPI_DATA_4_LINES;
    qspi_Drv->cmd.DummyCycles       = W25Q_DUMMY_6;  // As per datasheet
    qspi_Drv->cmd.NbData            = len;
    qspi_Drv->cmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
    qspi_Drv->cmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    qspi_Drv->cmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    HAL_StatusTypeDef status = HAL_QSPI_Command(&hqspi, &qspi_Drv->cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (status != 0)
    	return status;

    qspi_Drv->status &= ~QSPI_DMA_READ_COMPLETE;
    qspi_Drv->wait_dma_timeout = 100;

    if ( HAL_QSPI_Receive_DMA(&hqspi, data) )
    	return 1;
    while((qspi_Drv->status & QSPI_DMA_READ_COMPLETE) == 0)
    {
    	task_delay(1);
    	qspi_Drv->wait_dma_timeout--;
    	if ( qspi_Drv->wait_dma_timeout == 0 )
    		return 1;
    }

    return 0;
}

uint32_t W25Q128JV_EnableMemoryMappedMode(uint32_t *qspi_Drv_in)
{
    QSPI_DriverStruct_t *qspi_Drv = (QSPI_DriverStruct_t *)qspi_Drv_in;

    bzero(&qspi_Drv->cmd,sizeof(QSPI_CommandTypeDef));
    bzero(&qspi_Drv->mem_mapped_cfg,sizeof(QSPI_MemoryMappedTypeDef));

    // Read instruction
    qspi_Drv->cmd.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    qspi_Drv->cmd.Instruction       = W25Q128JV_CMD_FAST_READ_QUAD;
    qspi_Drv->cmd.AddressMode       = QSPI_ADDRESS_4_LINES;
    qspi_Drv->cmd.AddressSize       = QSPI_ADDRESS_24_BITS;
    qspi_Drv->cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    qspi_Drv->cmd.DataMode          = QSPI_DATA_4_LINES;
    qspi_Drv->cmd.DummyCycles       = W25Q_DUMMY_6;
    qspi_Drv->cmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
    qspi_Drv->cmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    qspi_Drv->cmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    qspi_Drv->mem_mapped_cfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
    qspi_Drv->mem_mapped_cfg.TimeOutPeriod     = 0;

    return HAL_QSPI_MemoryMapped(&hqspi, &qspi_Drv->cmd, &qspi_Drv->mem_mapped_cfg);
}

#endif // #ifdef STM32H7xx_HAL_QSPI_H
