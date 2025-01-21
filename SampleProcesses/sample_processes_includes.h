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
 * sample_processes_includes.h
 *
 *  Created on: Dec 4, 2024
 *      Author: fil
 */

#ifndef SAMPLEPROCESSES_SAMPLE_PROCESSES_INCLUDES_H_
#define SAMPLEPROCESSES_SAMPLE_PROCESSES_INCLUDES_H_

#define	SAMPLEPROCESS_1_BASIC	1
//#define	SAMPLEPROCESS_1_XMODEM	1
//#define	SAMPLEPROCESS_1_DCCPWM	1
//#define	SAMPLEPROCESS_1_DHTxx	1
//#define	SAMPLEPROCESS_1_QSPI	1
//#define	SAMPLEPROCESS_1_I2CMEM	1
//#define	SAMPLEPROCESS_1_ADC			1
//#define	SAMPLEPROCESS_UARTQUEUES	1
//#define	SAMPLEPROCESS_INTFLASH		1
//#define	SAMPLEPROCESS_I2CSENSORS		1

#ifdef	SAMPLEPROCESS_1_I2CMEM
	#define	AOS_STM32H7xx_HAL_I2C_H	1
#endif // #ifdef	SAMPLEPROCESS_1_I2CMEM


#endif /* SAMPLEPROCESSES_SAMPLE_PROCESSES_INCLUDES_H_ */
