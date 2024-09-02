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
 * s3_hex.c
 *
 *  Created on: Sep 2, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"
//#include "../../kernel/kernel_opt.h"

#ifdef MOTOROLA_HEX_DECODER_ENABLE

#include "s3_hex.h"
#include <string.h>

SYSTEM_RAM	S3Hex_struct_t	S3Hex;

uint8_t	*get_s3hex_data_ptr(void)
{
	return S3Hex.s3hex_line;
}

uint32_t get_s3hex_address(void)
{
	return S3Hex.address;
}

#endif
