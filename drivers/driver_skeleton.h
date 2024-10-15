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
 * driver_skeleton.h
 *
 *  Created on: Oct 14, 2024
 *      Author: fil
 */

#ifndef DRIVERS_DRIVER_SKELETON_H_
#define DRIVERS_DRIVER_SKELETON_H_

typedef struct
{
	uint8_t		status;
	uint8_t		state_machine;
	uint32_t	ticks;
	uint32_t	frequency;
	uint32_t	pulse_width;
	uint32_t	errors;
}driver_skel_Drv_TypeDef;

#endif /* DRIVERS_DRIVER_SKELETON_H_ */
