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
 * iir.h
 *
 *  Created on: Feb 19, 2024
 *      Author: fil
 *      https://github.com/YetAnotherElectronicsChannel/STM32_Calculating_IIR_Parameters/blob/master/code/Src/main.c
 */

#ifndef MODULES_AUDIO_IIR_H_
#define MODULES_AUDIO_IIR_H_

#define	IIR_HIGH_PASS	0
#define	IIR_LOW_PASS	1
#define	IIR_BAND_PASS	2
#define	IIR_NOTCH		3

#define	IIR_COEFF_A0	0
#define	IIR_COEFF_A1	1
#define	IIR_COEFF_A2	2
#define	IIR_COEFF_B1	3
#define	IIR_COEFF_B2	4

#endif /* MODULES_AUDIO_IIR_H_ */
