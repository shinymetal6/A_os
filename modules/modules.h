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
 * modules.h
 *
 *  Created on: Nov 16, 2024
 *      Author: fil
 */

#ifndef MODULES_MODULES_H_
#define MODULES_MODULES_H_

typedef struct
{
	uint8_t 			process;
	uint8_t				status;
	uint8_t				flags;
	uint8_t				handle;
	uint32_t			*private_data;
}MODULES_Struct_t;

#include "serial_transfers/xmodem_rx.h"
#include "hex_decoders/hex_decoders_common.h"
#include "hex_decoders/ihex.h"
#include "hex_decoders/hex_decoders_common.h"
#include "hex_decoders/s3_hex.h"
#include "modbus/modbus.h"
#include "sound_engine/sound_engine.h"
#include "midi_decoder/midi_decoder.h"
#include "pid/pid.h"

#endif /* MODULES_MODULES_H_ */
