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
	#define A_OS_STM32H743_PROCESSOR	1
	#define STM32_HAS_STREAM_DMA	1

	#ifdef STM32H7xx_HAL_UART_H
		#define A_OS_UART_ENABLED	1
	#endif
	#ifdef STM32H7xx_HAL_TIM_H
		#define A_OS_TIMERS_ENABLED	1
	#endif
	#ifdef STM32H7xx_HAL_ADC_H
		#define A_OS_ADC_CALIBRATION_3PARAMS	1
		#define A_OS_ADC_ENABLED	1
	#endif
	#ifdef STM32H7xx_HAL_DAC_H
		#define A_OS_DAC_ENABLED	1
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
	#define A_OS_STM32H753_PROCESSOR	1
	#define STM32_HAS_STREAM_DMA	1

	#ifdef STM32H7xx_HAL_UART_H
		#define A_OS_UART_ENABLED	1
	#endif
	#ifdef STM32H7xx_HAL_TIM_H
		#define A_OS_TIMERS_ENABLED	1
	#endif
	#ifdef STM32H7xx_HAL_ADC_H
		#define A_OS_ADC_ENABLED	1
		#define A_OS_ADC_CALIBRATION_3PARAMS	1
	#endif
	#ifdef STM32H7xx_HAL_DAC_H
		#define A_OS_DAC_ENABLED	1
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

#ifdef	STM32F722xx
	#define A_OS_STM32H722_PROCESSOR	1
	#define STM32_HAS_STREAM_DMA	1

	#ifdef STM32F7xx_HAL_UART_H
		#define A_OS_UART_ENABLED	1
	#endif
	#ifdef STM32F7xx_HAL_TIM_H
		#define A_OS_TIMERS_ENABLED	1
	#endif
	#ifdef STM32F7xx_HAL_ADC_H
		#define A_OS_ADC_CALIBRATION_3PARAMS	1
		#define A_OS_ADC_ENABLED	1
	#endif
	#ifdef STM32F7xx_HAL_DAC_H
		#define A_OS_DAC_ENABLED	1
	#endif
	#ifdef STM32F7xx_HAL_I2S_H
		#define A_OS_I2S_ENABLED	1
	#endif
	#ifdef STM32F7xx_HAL_RTC_H
		#define A_OS_RTC_ENABLED	1
	#endif
	#ifdef STM32F7xx_HAL_I2C_H
		#define A_OS_I2C_ENABLED	1
	#endif
	#ifdef STM32F7xx_HAL_SPI_H
		#define A_OS_SPI_ENABLED	1
	#endif
#endif

#ifdef	STM32L431xx
	#define A_OS_STM32L4xx_PROCESSOR	1
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

#ifdef	STM32G431xx
	#define A_OS_STM32G4xx_PROCESSOR	1
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


#ifdef	STM32G474xx
	#define A_OS_STM32G4xx_PROCESSOR	1
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
	#define A_OS_STM32G4xx_PROCESSOR	1
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

#ifdef	STM32U575xx
	#define STM32_HAS_STREAM_DMA	1

	#ifdef HAL_UART_MODULE_ENABLED
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
#endif // #ifdef	STM32U575xx

#ifdef	STM32F446xx
	#ifdef HAL_UART_MODULE_ENABLED
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
	#ifdef STM32G4xx_HAL_I2S_H
		#define A_OS_I2S_ENABLED	1
	#endif
	#ifdef STM32G4xx_HAL_RTC_H
		#define A_OS_RTC_ENABLED	1
	#endif
	#ifdef STM32G4xx_HAL_I2C_H
		#define A_OS_I2C_ENABLED	1
	#endif
	#ifdef HAL_SPI_MODULE_ENABLED
		#define A_OS_SPI_ENABLED	1
	#endif
#endif // #ifdef	STM32F446xx

#ifdef	STM32L152xE
	#ifdef HAL_UART_MODULE_ENABLED
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
	#ifdef STM32G4xx_HAL_I2S_H
		#define A_OS_I2S_ENABLED	1
	#endif
	#ifdef STM32G4xx_HAL_RTC_H
		#define A_OS_RTC_ENABLED	1
	#endif
	#ifdef STM32G4xx_HAL_I2C_H
		#define A_OS_I2C_ENABLED	1
	#endif
	#ifdef HAL_SPI_MODULE_ENABLED
		#define A_OS_SPI_ENABLED	1
	#endif
#endif // #ifdef	STM32L152xE
#ifdef	STM32H563xx
	#ifdef HAL_UART_MODULE_ENABLED
		#define A_OS_UART_ENABLED	1
	#endif
#endif // #ifdef	STM32H563xx
#ifdef	STM32F746xx
	#ifdef HAL_UART_MODULE_ENABLED
		#define A_OS_UART_ENABLED	1
	#endif
#endif // #ifdef	STM32F746xx

#endif /* BOARDS_IODEF_COMMON_H_ */
