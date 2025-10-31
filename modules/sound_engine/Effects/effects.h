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
 * effects.h
 *
 *  Created on: Oct 16, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_EFFECTS_EFFECTS_H_
#define MODULES_SOUND_EFFECTS_EFFECTS_H_

/* for limits -1.0F < v < 1.0F */
#define	FULL_SCALE_F_FACTOR			65535.0F
#define	HALF_SCALE_F_FACTOR			(FULL_SCALE_F_FACTOR / 2.0F)
#define	HUNDRED_SCALE_F_FACTOR		100
#define	HALF_HUNDRED_SCALE_F_FACTOR	(HUNDRED_SCALE_F_FACTOR / 2)


//#ifdef SOUND_ENGINE_I2S_ENABLED
#define __Q15_2_FLOAT(input) ((float)input / 32768.0F)
#define __FLOAT_2_Q15(output) ((q15_t)(output * 32768.0F))
/*
#else
#define __Q15_2_FLOAT(input) ((float)input / 65535.0F)
#define __FLOAT_2_Q15(output) ((q15_t)(output * 65535.0F))
#endif
*/

#endif /* MODULES_SOUND_EFFECTS_EFFECTS_H_ */
