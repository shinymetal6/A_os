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
 * fatfs.h
 *
 *  Created on: Nov 19, 2025
 *      Author: fil
 */

#ifndef DRIVERS_SDCARD_FATFS_H_
#define DRIVERS_SDCARD_FATFS_H_

#include "../../modules/fat/ff.h"
#include "../../modules/fat/ff_gen_drv.h"
#include "sd_diskio.h" /* defines SD_Driver as external */

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern uint8_t retSD; /* Return value for SD */
extern char SDPath[4]; /* SD logical drive path */
extern FATFS SDFatFS; /* File system object for SD logical drive */
extern FIL SDFile; /* File object for SD */

void MX_FATFS_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */
#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_SDCARD_FATFS_H_ */
