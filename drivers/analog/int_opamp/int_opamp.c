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
 * int_opamp.c
 *
 *  Created on: Jan 30, 2025
 *      Author: fil
 */
#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"
//#include "../../../kernel/kernel_opt.h"

#ifdef A_OS_OPAMP_ENABLED

#include "int_opamp.h"

uint8_t Int_SetOpAmpGain(OPAMP_HandleTypeDef *opamp,uint8_t gain)
{
	if ( gain < 8 )
	{
		opamp->Instance->CSR &= ~OPAMP_CSR_PGGAIN_Msk;
		opamp->Instance->CSR |= (gain << OPAMP_CSR_PGGAIN_Pos);
		return 0;
	}
	return 1;
}
#endif // #ifdef A_OS_OPAMP_ENABLED

