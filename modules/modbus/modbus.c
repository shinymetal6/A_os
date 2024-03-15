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
 * Project : u575_modbus 
*/
/*
 * modbus.c
 *
 *  Created on: Oct 5, 2023
 *      Author: fil
 */

#include "main.h"
#include "modbus.h"
#include "modbus_rtu.h"
#include "../../kernel/A_exported_functions.h"

A_modbus_t			A_modbus;
A_modbus_inout_t	A_modbus_inout;

#ifndef A_MODBUS_SW_CRC
// Internal init to be agnostic about IDE setup
uint8_t modbus_CRC_Init(void)
{
	hcrc.Instance = CRC;
	hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_DISABLE;
	hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_DISABLE;
	hcrc.Init.GeneratingPolynomial = 32773;
	hcrc.Init.CRCLength = CRC_POLYLENGTH_16B;
	hcrc.Init.InitValue = 0xffff;
	hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_BYTE;
	hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_ENABLE;
	hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
	return HAL_CRC_Init(&hcrc);
}
#endif

void modbus_init(uint32_t uart,uint8_t address,uint8_t *data_ptr,uint16_t data_size)
{
#ifndef A_MODBUS_SW_CRC
	HAL_CRC_DeInit(&hcrc);
	modbus_CRC_Init();
#endif
	A_modbus.modbus_addr = address;
	A_modbus.modbus_uart = uart;
	A_modbus.modbus_rx_packet_ptr = data_ptr;
	hw_receive_uart_sentinel(uart,A_modbus.modbus_rx_packet_ptr,1024,address, address,MODBUS_TIMEOUT);
}

uint8_t modbus_process(void)
{
uint16_t len;
	len = hw_get_uart_receive_len(A_modbus.modbus_uart);
	return rtu_modbus_process(A_modbus.modbus_rx_packet_ptr,len);
}

uint8_t modbus_get_coil(uint16_t coil_number)
{
uint8_t tmp_coil;
	tmp_coil = A_modbus_inout.coils[coil_number>>3];
	tmp_coil >>= (coil_number & 0x07);
	return tmp_coil & 0x01;
}
