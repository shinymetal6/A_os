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
 * qspi.h
 *
 *  Created on: Nov 17, 2025
 *      Author: fil
 */

#ifndef DRIVERS_QSPI_QSPI_H_
#define DRIVERS_QSPI_QSPI_H_

#ifdef A_OS_SPI_ENABLED

typedef struct
{
	/* driver header */
	uint8_t				status;
	uint8_t				flags;
	uint8_t 			process;
	QSPI_HandleTypeDef 	*qspi;
	uint8_t				qspi_id;
	uint32_t 			wakeup_id;
	uint32_t			*next_drv;
	/* driver proprietary data */
	uint8_t				man_id, dev_id,capacity;
	QSPI_CommandTypeDef cmd;
	QSPI_MemoryMappedTypeDef mem_mapped_cfg;
	/* Internals */
	uint32_t			(*read)  ( uint32_t *qspi_Drv, uint32_t address,uint8_t *data,uint32_t data_len);
	uint32_t			(*write) ( uint32_t *qspi_Drv, uint32_t address,uint8_t *data,uint32_t data_len);
	uint32_t			(*erase_sector) ( uint32_t *qspi_Drv,uint32_t sector_addr);
	uint32_t			(*get_id) (uint32_t *qspi_Drv);
	uint32_t			(*memory_map) (uint32_t *qspi_Drv);
	uint32_t			(*chip_erase) (uint32_t *qspi_Drv);
	uint32_t			(*chip_reset) (uint32_t *qspi_Drv);
	uint8_t				wait_dma_timeout;
} QSPI_DriverStruct_t;
/* status */
#define	QSPI_DMA_ERROR			0x02
#define	QSPI_DMA_READ_COMPLETE	0x04
#define	QSPI_DMA_WRITE_COMPLETE	0x08
/* flags */
#define	QSPI_FLAGS_WAKEUP		0x80


#define	QSPI_25XX	0x01

extern uint32_t	qspi_register(QSPI_DriverStruct_t *qspi_Drv);
extern uint32_t	qspi_read (QSPI_DriverStruct_t *qspi_Drv, uint32_t address,uint8_t *data,uint32_t data_len);
extern uint32_t	qspi_write(QSPI_DriverStruct_t *qspi_Drv, uint32_t address,uint8_t *data,uint32_t data_len);
extern uint32_t	qspi_erase_sectors(QSPI_DriverStruct_t *qspi_Drv,uint32_t sector_addr);
extern uint32_t	qspi_chip_erase(QSPI_DriverStruct_t *qspi_Drv);
extern uint32_t	qspi_get_id(QSPI_DriverStruct_t *qspi_Drv);
extern uint32_t	qspi_memory_map(QSPI_DriverStruct_t *qspi_Drv);
extern uint32_t	qspi_reset_chip(QSPI_DriverStruct_t *qspi_Drv);

#endif // #ifdef A_OS_SPI_ENABLED

#endif /* DRIVERS_QSPI_QSPI_H_ */
