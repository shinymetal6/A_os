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
 * hw_qspi.c
 *
 *  Created on: Sep 26, 2023
 *      Author: fil
 */

#include "main.h"
#include "../A.h"
#include "../scheduler.h"
#include "../A_exported_functions.h"
#include "../hwmanager.h"
#include "../system_default.h"
#include "hw_qspi.h"

#if defined QSPI_ENABLED

extern	HWMngr_t	HWMngr[PERIPHERAL_NUM];
extern	Asys_t		Asys;

uint8_t A_qspi_enable_write(QSPI_HandleTypeDef *A_hqspi,uint32_t BlockAddress)
{
	if ( HWMngr[HW_QSPI].process != Asys.current_process )
		return HW_QSPI_ERROR_HW_NOT_OWNED;
	 return qspi_WriteEnable(A_hqspi);
}

uint8_t A_qspi_disable_write(QSPI_HandleTypeDef *A_hqspi,uint32_t BlockAddress)
{
	if ( HWMngr[HW_QSPI].process != Asys.current_process )
		return HW_QSPI_ERROR_HW_NOT_OWNED;
	 return qspi_WriteDisable(A_hqspi);
}

uint8_t A_qspi_erase_block(QSPI_HandleTypeDef *A_hqspi,uint32_t BlockAddress)
{
	if ( HWMngr[HW_QSPI].process != Asys.current_process )
		return HW_QSPI_ERROR_HW_NOT_OWNED;
	 return qspi_Erase_Block(A_hqspi,BlockAddress);
}

uint8_t A_qspi_erase_chip(QSPI_HandleTypeDef *A_hqspi)
{
	if ( HWMngr[HW_QSPI].process != Asys.current_process )
		return HW_QSPI_ERROR_HW_NOT_OWNED;
	 return qspi_Erase_Chip(A_hqspi);
}

uint8_t A_qspi_write(QSPI_HandleTypeDef *A_hqspi,uint8_t* data,uint32_t addr, uint32_t size)
{
	if ( HWMngr[HW_QSPI].process != Asys.current_process )
		return HW_QSPI_ERROR_HW_NOT_OWNED;
	return qspi_Write(A_hqspi,data, addr, size);
}

uint8_t A_qspi_read(QSPI_HandleTypeDef *A_hqspi,uint8_t* data,uint32_t addr, uint32_t size)
{
	if ( HWMngr[HW_QSPI].process != Asys.current_process )
		return HW_QSPI_ERROR_HW_NOT_OWNED;
	return qspi_Read(A_hqspi,data, addr, size);
}

QSPI_HandleTypeDef *A_qspi_init(void)
{
	if ( HWMngr[HW_QSPI].process != Asys.current_process )
		return 0;
	return qspi_init();
}

#endif




