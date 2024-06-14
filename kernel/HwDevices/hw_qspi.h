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
 * Project : A_os_F746gdisco_devel 
*/
/*
 * hw_qspi.h
 *
 *  Created on: Sep 26, 2023
 *      Author: fil
 */

#ifndef KERNEL_HWDEVICES_HW_QSPI_H_
#define KERNEL_HWDEVICES_HW_QSPI_H_
#if defined QSPI_ENABLED

#include "../../drivers/external_flash/qspi.h"

extern	QSPI_HandleTypeDef *qspi_init(void);
extern	uint8_t A_qspi_enable_write(void);
extern	uint8_t A_qspi_disable_write(void);
extern	uint8_t A_qspi_EraseBlockByNumber(uint32_t BlockNumber);
extern	uint8_t A_qspi_EraseBlockByAddress(uint32_t BlockAddress);
extern	uint8_t A_qspi_erase_chip(void);
extern	uint8_t A_qspi_write(uint8_t* data,uint32_t addr, uint32_t size);
extern	uint8_t A_qspi_read(uint8_t* data,uint32_t addr, uint32_t size);
extern	QSPI_HandleTypeDef *A_qspi_init(void);

#endif
#define HW_QSPI_ERROR_NONE			0
#define HW_QSPI_ERROR_HW_NOT_OWNED	1

#endif /* KERNEL_HWDEVICES_HW_QSPI_H_ */
