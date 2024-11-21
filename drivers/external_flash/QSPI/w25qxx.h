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
 * w25qxx.h
 *
 *  Created on: Nov 21, 2024
 *      Author: fil
 */

#ifndef DRIVERS_EXTERNAL_FLASH_QSPI_W25QXX_H_
#define DRIVERS_EXTERNAL_FLASH_QSPI_W25QXX_H_

typedef struct
{
	uint8_t				status;
	uint8_t				flags;
	uint8_t				*data;
	QSPI_HandleTypeDef 	*qspi_bus;
	uint32_t 			wakeup_id;
	uint32_t 			timeout;
	uint8_t				qspi_id;
	uint8_t				qspi_status_reg;
	uint8_t				qspi_status_reg1;
	uint8_t				qspi_status_reg2;
	uint8_t				qspi_status_reg3;
	uint32_t 			FlashSize;          /*!< Size of the flash */
	uint32_t 			EraseSectorSize;    /*!< Size of sectors for the erase operation */
	uint32_t 			EraseSectorsNumber; /*!< Number of sectors for the erase operation */
	uint32_t 			ProgPageSize;       /*!< Size of pages for the program operation */
	uint32_t 			ProgPagesNumber;    /*!< Number of pages for the program operation */
	uint32_t 			BlockSize;    	   	/*!< Size of the block */
	QSPI_CommandTypeDef com;
}W25Qxx_Drv_TypeDef;

/* status */
#define	QSPI_BUSY					0x80
#define	QSPI_WEL					0x40
#define	QSPI_READ_COMPLETE			0x20
#define	QSPI_WRITE_COMPLETE			0x10
#define	QSPI_ERROR					0x01

/* flags */
#define	QSPI_USES_DMA				0x80
#define	QSPI_WAKEUP_ON_READ			0x20
#define	QSPI_WAKEUP_ON_WRITE		0x10

extern uint32_t	w25qxx_register(W25Qxx_Drv_TypeDef *driver_private_data);

#endif /* DRIVERS_EXTERNAL_FLASH_QSPI_W25QXX_H_ */
