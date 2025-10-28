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
 * i2c.c
 *
 *  Created on: Oct 28, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#ifdef A_OS_I2C_ENABLED

#include "i2c.h"
I2C_DriverStruct_t	*i2c_drv_ptr;

#endif //#ifdef A_OS_I2C_ENABLED

