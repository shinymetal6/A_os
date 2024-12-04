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
 * audio.c
 *
 *  Created on: Nov 22, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/kernel_opt.h"

#include "audio.h"
#include "effects.h"

#ifdef AUDIO_GENERATORS_ENABLED

AUDIO_FAST_RAM	int16_t				pipe[MAX_EFFECTS] [HALF_NUMBER_OF_AUDIO_SAMPLES];

extern int16_t	oscout_buffer[HALF_NUMBER_OF_AUDIO_SAMPLES];


#endif // #ifdef AUDIO_GENERATORS_ENABLED



