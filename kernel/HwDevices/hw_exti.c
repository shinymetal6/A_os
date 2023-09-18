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
 * hw_exti.c
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */

#include "main.h"
#include "hw_spi.h"
#include "../A.h"
#include "../scheduler.h"
#include "../A_exported_functions.h"
#include "../hwmanager.h"
#include "../system_default.h"

#if defined NFC_ENABLED

extern	void st25r3916Isr( void );

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
	if ( GPIO_Pin == ST25R391X_INT_Pin)
		st25r3916Isr();
}
#endif
