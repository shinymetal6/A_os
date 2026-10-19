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

//#define	SAMPLEPROCESS_1_BASIC		1
//#define	SAMPLEPROCESS_1_BASIC_DAC	1
//#define	SAMPLEPROCESS_1_USB_XMODEM	1
#ifdef SAMPLEPROCESS_1_USB_XMODEM
	#define USB_DEVICE_ENABLED	1
#endif //#ifdef SAMPLEPROCESS_1_USB_XMODEM
#define	SAMPLEPROCESS_1_UART_XMODEM		1
//#define	SAMPLEPROCESS_1_USB_TEST	1
#ifdef SAMPLEPROCESS_1_USB_TEST
	#define USB_DEVICE_ENABLED	1
#endif //#ifdef SAMPLEPROCESS_1_USB_TEST

//#define	SAMPLEPROCESS_1_SDCARD_WAV_READER	1
#ifdef SAMPLEPROCESS_1_SDCARD_WAV_READER
	#define A_OS_WAV_PLAYER_ENABLED	1
#endif //#ifdef SAMPLEPROCESS_1_SDCARD_WAV_READER


#endif /* SAMPLEPROCESSES_SAMPLE_PROCESSES_INCLUDES_H_ */
