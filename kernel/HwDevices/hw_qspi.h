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

extern	uint8_t qspi_Erase_Chip(QSPI_HandleTypeDef *QSPIHandle);
extern	uint8_t qspi_Erase_Block(QSPI_HandleTypeDef *QSPIHandle,uint32_t BlockAddress);
extern	uint8_t qspi_Write(QSPI_HandleTypeDef *QSPIHandle,uint8_t* pData, uint32_t WriteAddr, uint32_t Size);
extern	uint8_t qspi_Read(QSPI_HandleTypeDef *QSPIHandle,uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
extern	uint8_t qspi_WriteEnable(QSPI_HandleTypeDef *QSPIHandle);
extern	uint8_t qspi_WriteDisable(QSPI_HandleTypeDef *QSPIHandle);

extern	QSPI_HandleTypeDef *qspi_init(void);
#endif
#define HW_QSPI_ERROR_NONE			0
#define HW_QSPI_ERROR_HW_NOT_OWNED	1

#endif /* KERNEL_HWDEVICES_HW_QSPI_H_ */
