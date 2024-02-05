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
 * modbus.h
 *
 *  Created on: Oct 5, 2023
 *      Author: fil
 */

#ifndef MODBUS_H_
#define MODBUS_H_

extern	UART_HandleTypeDef 					huart1;
#define	MODBUS_UART							huart1

//#define	MODBUS_SW_CRC							1
#ifndef MODBUS_SW_CRC
extern	CRC_HandleTypeDef 					hcrc;
#define	MODBUS_CRC							hcrc
#endif

#define	MODBUS_EOP_TIMER					10
#define	MODBUS_TIMEOUT						100

#define	MODBUS_PKT_SIZE						513
#define MODBUS_BROADCAST_ADDRESS    		0

#define	MODBUS_READ_COIL					0x01
#define	MODBUS_READ_DISCRETE_INPUT			0x02
#define	MODBUS_READ_REGISTER				0x03
#define	MODBUS_READ_INPUT_REGISTER			0x04
#define	MODBUS_WRITE_COIL					0x05
#define	MODBUS_WRITE_REGISTER				0x06
#define	MODBUS_WRITE_MULTIPLE_COILS			0x0f
#define	MODBUS_WRITE_MULTIPLE_REGISTERS		0x10
#define MODBUS_FC_REPORT_SLAVE_ID			0x11

#define MODBUS_FC_MASK_WRITE_REGISTER       0x16
#define MODBUS_FC_WRITE_AND_READ_REGISTERS  0x17

typedef struct {
	uint8_t		modbus_addr;
	uint8_t		*modbus_rx_packet_ptr;
	uint8_t		modbus_uart;
	uint8_t		modbus_tx_packet[MODBUS_PKT_SIZE];
	uint8_t		modbus_tx_packet_len;
	uint16_t 	modbus_crc;
} A_modbus_t;

/* modbus_state */
#define	MODBUS_RTU_MODE					0x80

#define	NUM_MUL_REG						4

#define	A_MAX_COILS						1000
#define	A_MAX_DISCRETE_IN				1000
#define	A_MAX_REGS						1000

typedef struct {
	uint8_t		bit_index;
	uint8_t		coils[(A_MAX_COILS/8)];
	uint8_t		temp_coils_icoils[(A_MAX_COILS/8)];
	uint8_t		discrete_input[(A_MAX_DISCRETE_IN/8)];
	uint8_t		holding_registers[A_MAX_REGS];
	uint8_t		input_registers[A_MAX_REGS];
	uint8_t		multiple_registers[A_MAX_REGS];
	uint8_t		multiple_input_registers[A_MAX_REGS];
} A_modbus_inout_t;

#endif /* MODBUS_H_ */
