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
 * qspi_w25q128.h
 *
 *  Created on: Nov 17, 2025
 *      Author: fil
 */

#ifndef DRIVERS_QSPI_QSPI_W25Q128_H_
#define DRIVERS_QSPI_QSPI_W25Q128_H_

// W25Q128JV Commands
#define W25Q128JV_CMD_READ_ID          0x9F
#define W25Q128JV_CMD_WRITE_ENABLE     0x06
#define W25Q128JV_CMD_READ_STATUS1     0x05
#define W25Q128JV_CMD_PAGE_PROGRAM     0x02
#define W25Q128JV_CMD_SECTOR_ERASE     0x20
#define W25Q128JV_CMD_BLOCK_ERASE_64K  0xD8
#define W25Q128JV_CMD_CHIP_ERASE       0xC7
#define W25Q128JV_CMD_FAST_READ_QUAD   0xEB
#define W25Q128JV_CMD_ENTER_QPI        0x38

// Sizes
#define W25Q128JV_SECTOR_SIZE    4096
#define W25Q128JV_PAGE_SIZE      256
#define W25Q128JV_CAPACITY_BYTES (16 * 1024 * 1024) // 16 MB

// Status register masks
#define W25Q128JV_BUSY_MASK      0x01
#define W25Q128JV_WEL_MASK       0x02

#define W25Q128JV_TICK_TIMEOUT      100

#define	W25Q_DUMMY_0						0
#define	W25Q_DUMMY_6						6

// QSPI Handle extern
extern QSPI_HandleTypeDef hqspi;

// Functions
uint32_t W25Q128JV_ReadID(uint32_t *qspi_Drv_in);
// Erase
uint32_t W25Q128JV_EraseSector(uint32_t *qspi_Drv_in,uint32_t sector_addr);
// Program & Read
uint32_t W25Q128JV_PagesProgram(uint32_t *qspi_Drv_in,uint32_t address, uint8_t* data, uint32_t len);
uint32_t W25Q128JV_FastReadQuad(uint32_t *qspi_Drv_in,uint32_t address, uint8_t* data, uint32_t len);
// Memory-mapped mode
uint32_t W25Q128JV_EnableMemoryMappedMode(uint32_t *qspi_Drv_in);


#endif /* DRIVERS_QSPI_QSPI_W25Q128_H_ */
