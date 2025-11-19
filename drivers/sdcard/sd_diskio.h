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
 * sd_diskio.h
 *
 *  Created on: Nov 19, 2025
 *      Author: fil
 */

#ifndef DRIVERS_SDCARD_SD_DISKIO_H_
#define DRIVERS_SDCARD_SD_DISKIO_H_

#include "sdcard.h"
#include "../../modules/fat/ff_gen_drv.h"

#define SD_TIMEOUT 30 * 1000
#define SD_DEFAULT_BLOCK_SIZE 512

extern const Diskio_drvTypeDef  SD_Driver;

#endif /* DRIVERS_SDCARD_SD_DISKIO_H_ */
