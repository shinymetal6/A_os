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
 * iodef_common.h
 *
 *  Created on: Dec 21, 2024
 *      Author: fil
 */
#ifndef BOARDS_IODEF_COMMON_H_
#define BOARDS_IODEF_COMMON_H_

#include "main.h"

#undef   STM32_HAS_STREAM_DMA

#ifdef	STM32H743xx

	#define STM32_HAS_STREAM_DMA	1

	#ifdef STM32H7xx_HAL_UART_H
		#define A_OS_UART_ENABLED	1
	#endif
	#ifdef STM32H7xx_HAL_TIM_H
		#define A_OS_TIMERS_ENABLED	1
	#endif
	#ifdef STM32H7xx_HAL_ADC_H
		#define A_OS_ADC_ENABLED	1
	#endif
	#ifdef STM32H7xx_HAL_DAC_H
		#define A_OS_DAC_ENABLED	1
	#endif
	#ifdef STM32H7xx_HAL_OPAMP_H
		#define A_OS_OPAMP_ENABLED	1
	#endif
	#ifdef STM32H7xx_HAL_I2S_H
		#define A_OS_I2S_ENABLED	1
	#endif
	#ifdef STM32H7xx_HAL_RTC_H
		#define A_OS_RTC_ENABLED	1
	#endif
	#ifdef STM32H7xx_HAL_I2C_H
		#define A_OS_I2C_ENABLED	1
	#endif
	#ifdef STM32H7xx_HAL_SPI_H
		#define A_OS_SPI_ENABLED	1
	#endif
#endif

#ifdef	STM32H753xx
	#define STM32_HAS_STREAM_DMA	1

	#ifdef STM32H7xx_HAL_UART_H
		#define A_OS_UART_ENABLED	1
	#endif
	#ifdef STM32H7xx_HAL_TIM_H
		#define A_OS_TIMERS_ENABLED	1
	#endif
	#ifdef STM32H7xx_HAL_ADC_H
		#define A_OS_ADC_ENABLED	1
	#endif
	#ifdef STM32H7xx_HAL_DAC_H
		#define A_OS_DAC_ENABLED	1
	#endif
	#ifdef STM32H7xx_HAL_OPAMP_H
		#define A_OS_OPAMP_ENABLED	1
	#endif
	#ifdef STM32H7xx_HAL_I2S_H
		#define A_OS_I2S_ENABLED	1
	#endif
	#ifdef STM32H7xx_HAL_RTC_H
		#define A_OS_RTC_ENABLED	1
	#endif
	#ifdef STM32H7xx_HAL_I2C_H
		#define A_OS_I2C_ENABLED	1
	#endif
	#ifdef STM32H7xx_HAL_SPI_H
		#define A_OS_SPI_ENABLED	1
	#endif
#endif

#ifdef	STM32L431xx
	#ifdef STM32L4xx_HAL_UART_H
		#define A_OS_UART_ENABLED	1
	#endif
	#ifdef STM32L4xx_HAL_TIM_H
		#define A_OS_TIMERS_ENABLED	1
	#endif
	#ifdef STM32L4xx_HAL_ADC_H
		#define A_OS_ADC_ENABLED	1
	#endif
	#ifdef STM32L4xx_HAL_DAC_H
		#define A_OS_DAC_ENABLED	1
	#endif
	#ifdef STM32L4xx_HAL_OPAMP_H
		#define A_OS_OPAMP_ENABLED	1
	#endif
	#ifdef STM32L4xx_HAL_I2S_H
		#define A_OS_I2S_ENABLED	1
	#endif
	#ifdef STM32L4xx_HAL_RTC_H
		#define A_OS_RTC_ENABLED	1
	#endif
	#ifdef STM32L4xx_HAL_I2C_H
		#define A_OS_I2C_ENABLED	1
	#endif
	#ifdef STM32L4xx_HAL_SPI_H
		#define A_OS_SPI_ENABLED	1
	#endif
#endif

#ifdef	STM32G474xx
	#define STM32G4xx_USB	1
	#ifdef STM32G4xx_HAL_UART_H
		#define A_OS_UART_ENABLED	1
	#endif
	#ifdef STM32G4xx_HAL_TIM_H
		#define A_OS_TIMERS_ENABLED	1
	#endif
	#ifdef STM32G4xx_HAL_ADC_H
		#define A_OS_ADC_ENABLED	1
	#endif
	#ifdef STM32G4xx_HAL_DAC_H
		#define A_OS_DAC_ENABLED	1
	#endif
	#ifdef STM32G4xx_HAL_OPAMP_H
		#define A_OS_OPAMP_ENABLED	1
	#endif
	#ifdef STM32G4xx_HAL_I2S_H
		#define A_OS_I2S_ENABLED	1
	#endif
	#ifdef STM32G4xx_HAL_RTC_H
		#define A_OS_RTC_ENABLED	1
	#endif
	#ifdef STM32G4xx_HAL_I2C_H
		#define A_OS_I2C_ENABLED	1
	#endif
	#ifdef STM32G4xx_HAL_SPI_H
		#define A_OS_SPI_ENABLED	1
	#endif
#endif


#ifdef	STM32G491xx
	#ifdef STM32G4xx_HAL_UART_H
		#define A_OS_UART_ENABLED	1
	#endif
	#ifdef STM32G4xx_HAL_TIM_H
		#define A_OS_TIMERS_ENABLED	1
	#endif
	#ifdef STM32G4xx_HAL_ADC_H
		#define A_OS_ADC_ENABLED	1
	#endif
	#ifdef STM32G4xx_HAL_DAC_H
		#define A_OS_DAC_ENABLED	1
	#endif
	#ifdef STM32G4xx_HAL_OPAMP_H
		#define A_OS_OPAMP_ENABLED	1
	#endif
	#ifdef STM32G4xx_HAL_I2S_H
		#define A_OS_I2S_ENABLED	1
	#endif
	#ifdef STM32G4xx_HAL_RTC_H
		#define A_OS_RTC_ENABLED	1
	#endif
	#ifdef STM32G4xx_HAL_I2C_H
		#define A_OS_I2C_ENABLED	1
	#endif
	#ifdef STM32G4xx_HAL_SPI_H
		#define A_OS_SPI_ENABLED	1
	#endif
#endif
#endif /* BOARDS_IODEF_COMMON_H_ */
