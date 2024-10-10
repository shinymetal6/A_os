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
 * drivers_common.h
 *
 *  Created on: Oct 10, 2024
 *      Author: fil
 */

#ifndef DRIVERS_DRIVERS_COMMON_H_
#define DRIVERS_DRIVERS_COMMON_H_

#include "../kernel/A.h"

typedef struct
{
	uint8_t 	process;
	uint8_t		status;
	uint8_t		flags;
	char		driver_name[32];
	void 		(*before_check_timers_callback)(void);
	void 		(*after_check_timers_callback)(void);
}DriversDefs_t;
/* status */
#define	DRIVER_IN_USE	1
#define	DRIVER_UNUSED	0

extern	DriversDefs_t	DriversDefs[NUM_MAX_DRIVERS];

extern	uint32_t	driver_register(DriversDefs_t *driver);
extern	uint32_t	driver_unregister(DriversDefs_t *driver);


#endif /* DRIVERS_DRIVERS_COMMON_H_ */
