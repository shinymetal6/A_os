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
	/* driver header */
	uint8_t				status;
	uint8_t				flags;
	uint8_t 			process;
	QSPI_HandleTypeDef 	*qspi_bus;
	uint8_t				qspi_id;
	uint32_t 			wakeup_id;
	uint32_t			*next_drv;
	/* driver proprietary data */
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
	/* Internals */
	/*
	uint32_t			(*read)  (uint32_t *w25qxx_Drv_e, uint32_t address,uint8_t *data,uint32_t data_len);
	uint32_t			(*write) ( uint32_t *w25qxx_Drv_e,uint32_t address,uint8_t *data,uint32_t data_len);
	uint32_t			(*erase_blocks) ( uint32_t *w25qxx_Drv_e,uint32_t start_block, uint32_t number_of_blocks);
	uint32_t			(*erase_sectors) ( uint32_t *w25qxx_Drv_e,uint32_t start_sector, uint32_t number_of_sectors);
	uint32_t			(*erase_chip) (uint32_t *w25qxx_Drv_e);
	uint32_t			(*get_id) (uint32_t *w25qxx_Drv_e,uint8_t *data);
	uint32_t			(*get_status) (uint32_t *w25qxx_Drv_e);
	uint32_t			(*get_flags) (uint32_t *w25qxx_Drv_e);
	uint32_t			(*set_flags) (uint32_t *w25qxx_Drv_e, uint32_t flags);
	*/
}W25Qxx_DriverStruct_t;

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

extern uint32_t w25qxx_read(uint32_t *w25qxx_Drv_e, uint32_t address,uint8_t *data,uint32_t data_len);
extern uint32_t w25qxx_write(uint32_t *w25qxx_Drv_e, uint32_t address,uint8_t *data,uint32_t data_len);
extern uint32_t w25qxx_erasesectors(uint32_t *w25qxx_Drv_e, uint32_t start_sector, uint32_t number_of_sectors);
extern uint32_t w25qxx_eraseblocks(uint32_t *w25qxx_Drv_e, uint32_t start_block, uint32_t number_of_blocks);
extern uint32_t w25qxx_erasechip(uint32_t *w25qxx_Drv_e);
extern uint32_t w25qxx_GetID(uint32_t *w25qxx_Drv_e, uint8_t *data);
extern uint32_t w25qxx_ReadStatusReg(uint32_t *w25qxx_Drv,uint8_t reg,uint16_t timeout);
extern uint32_t w25qxx_WriteStatusReg(uint32_t *w25qxx_Drv,uint8_t reg,uint16_t timeout);
extern uint32_t	w25qxx_ReadAllStatusRegs(uint32_t *w25qxx_Drv_e,uint16_t timeout);


#endif /* DRIVERS_EXTERNAL_FLASH_QSPI_W25QXX_H_ */
