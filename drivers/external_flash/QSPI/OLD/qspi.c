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
 * Project : A_Libraries
*/
/*
 * qspi.c
 *
 *  Created on: Sep 26, 2023
 *      Author: fil
 */
#include "main.h"
#include "../../kernel/A.h"
#include "../../kernel/system_default.h"

#if defined QSPI_ENABLED

#include "qspi.h"
QSPI_Info	QSPI_memory;

extern	Asys_t			Asys;
#ifdef QSPI_WINBOND
#include "w25q.h"
#endif
QSPI_HandleTypeDef *qspi_init(void)
{
#ifdef QSPI_WINBOND
	Asys.qspi_id = w25q_Init();
	QSPI_memory.FlashSize = MEM_FLASH_SIZE*1024*1024;
	QSPI_memory.EraseSectorSize = MEM_SECTOR_SIZE*1024;
	QSPI_memory.EraseSectorsNumber = SECTOR_COUNT;
	QSPI_memory.ProgPageSize = MEM_PAGE_SIZE;
	QSPI_memory.ProgPagesNumber = PAGE_COUNT;
	QSPI_memory.BlockSize = w25q_GetSBlockSize();
#endif
	return &HQSPI1;
}

uint8_t qspi_WriteEnable(void)
{
#ifdef QSPI_WINBOND
	return w25q_WriteEnable(1);
#endif
}

uint8_t qspi_WriteDisable(void)
{
#ifdef QSPI_WINBOND
	return w25q_WriteEnable(0);
#endif
}

uint8_t qspi_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size)
{
#ifdef QSPI_WINBOND
	//return w25q_ReadDataByAddress(pData, ReadAddr, Size);
	return w25q_ReadRaw(pData, ReadAddr, Size);
#endif
}

uint8_t qspi_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size)
{
#ifdef QSPI_WINBOND
	return w25q_ProgramRaw(pData, WriteAddr, Size);
#endif
}

uint8_t qspi_EraseBlockByNumber(uint32_t BlockNumber)
{
#ifdef QSPI_WINBOND
	return w25q_EraseBlockByNumber(BlockNumber);
#endif
}

uint8_t qspi_EraseBlockByAddress(uint32_t BlockAddress)
{
#ifdef QSPI_WINBOND
	return w25q_EraseBlockByAddress(BlockAddress);
#endif
}

uint8_t qspi_Erase_Sector(uint32_t SectorAddress)
{
#ifdef QSPI_WINBOND
	return w25q_EraseSector(SectorAddress);
#endif
}

uint8_t qspi_Erase_Chip(void)
{
#ifdef QSPI_WINBOND
	return w25q_EraseChip();
#endif
}

uint32_t qspi_GetBlockSize(void)
{
#ifdef QSPI_WINBOND
	return w25q_GetBlockSize();
#endif
}

#endif
