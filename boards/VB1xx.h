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
 * VB1xx.h
 *
 *  Created on: Feb 16, 2024
 *      Author: fil
 */

#ifndef BOARDS_VB1XX_H_
#define BOARDS_VB1XX_H_

/* Memories */

// memory pool defines
extern	uint8_t					*_mempool_start,*_mempool_end;
#define	POOL_START			    (uint32_t )(&_mempool_start)
#define	POOL_END			    (uint32_t )(&_mempool_end)
#define	POOL_CHUNK_SIZE		    1024
#define	POOL_SIZE			    (POOL_END - POOL_START)
// POOL_NUM must be a constant value to compile
#define	POOL_NUM			    84

// system defines
extern	uint8_t					*_osSysRam_start,*_osSysRam_end;
#define SRAM_START				(uint32_t )(&_osSysRam_start)
#define SRAM_END				(uint32_t )(&_osSysRam_end)
#define SRAM_SIZE				(&_osSysRam_end - &_osSysRam_start)
#define SIZE_SCHED_STACK		2048U
#define SCHED_STACK_START		SRAM_END

#define IDLE_STACK_START		(uint32_t )(&_osSysRam_end - SIZE_SCHED_STACK)
#define SIZE_IDLE_STACK          2048U
#define	FIRST_PRC_STACK_START	 (IDLE_STACK_START - SIZE_IDLE_STACK)

/* I/O */

#define	LED_1_GPIOPORT			LED_GPIO_Port
#define	LED_1_GPIOBIT			LED_Pin

/* Clock */
#define TICK_HZ 				1000U
#define HSI_CLOCK         		480000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK
/* Others */
#define	ASSIGNED				1

extern	UART_HandleTypeDef 	huart4;
#define	CONSOLE				huart4

#define	POWERSAVE_ENABLED		1
#define	QSPI_ENABLED			1
#define	SDCARD_ENABLED			1
//#define	WIRELESS_ENABLED		1
#define	LCD_096_ENABLED			1
//#define	LCD_2I8_ENABLED			1
#define	ENCODER_ENABLED			1
#define	CODEC_ENABLED			1
#define	CODEC_NAU88C22			1
#define	HWRANDOM_GEN			1
#define	ADC_ENABLED				1
#define	DAC_ENABLED				1
#define	USB_ENABLED				1
#define	USE_ITCM				1
#define	I2STX_PRIORITY			7
#define	I2SRX_PRIORITY			7

#ifdef USB_ENABLED
	//#define	USB_CDC				1
	#define	USB_MIDI			1
#endif // #ifdef USB_ENABLED

#ifdef QSPI_ENABLED
	#define	QSPI_WINBOND			1
	extern	QSPI_HandleTypeDef 	hqspi;
	#define	HQSPI1				hqspi
#endif // #ifdef QSPI_ENABLED


#ifdef SDCARD_ENABLED
	#define	MICROSD_DETECT_IOPORT	SD_DETECT_GPIO_Port
	#define	MICROSD_DETECT_PIN		SD_DETECT_Pin
#endif // #ifdef SDCARD_ENABLED

#ifdef ADC_ENABLED
		extern	ADC_HandleTypeDef 	hadc1;
		extern	ADC_HandleTypeDef 	hadc2;
		extern	TIM_HandleTypeDef 	htim6;
		#define	CONTROL_ADC1		hadc1
		#define	ANALOG_IN_ADC2		hadc2
		#define	CONTROL_TIMER		htim6
#endif // #ifdef ADC_DAC_ENABLED

#ifdef DAC_ENABLED
	extern	DAC_HandleTypeDef hdac1;
#endif // #ifdef DAC_ENABLED

#ifdef LCD_096_ENABLED
	extern SPI_HandleTypeDef hspi1;
	#define ST7735_SPI_PORT 					hspi1
	#define ST7735_SPI_TIMEOUT					2000

	#define ST7735_CS_GPIO_Port					LCD_SS_GPIO_Port
	#define ST7735_CS_Pin						LCD_SS_Pin

	#define ST7735_DC_GPIO_Port					LCD_DC_GPIO_Port
	#define ST7735_DC_Pin						LCD_DC_Pin

	#define ST7735_RES_GPIO_Port				LCD_RESET_GPIO_Port
	#define ST7735_RES_Pin						LCD_RESET_Pin
#endif // #ifdef LCD_096_ENABLED

#ifdef LCD_2I8_ENABLED
	extern SPI_HandleTypeDef hspi1;
	#define ILI9341_SPI_PORT 					hspi1
	#define ILI9341_TOUCH_SPI_PORT				hspi1
	#define ILI9341_SPI_TIMEOUT					2000
	#define ILI9341_RES_Pin       				LCD_RESET_Pin
	#define ILI9341_RES_GPIO_Port 				LCD_RESET_GPIO_Port
	#define ILI9341_CS_Pin        				LCD_SS_Pin
	#define ILI9341_CS_GPIO_Port  				LCD_SS_GPIO_Port
	#define ILI9341_DC_Pin        				LCD_DC_Pin
	#define ILI9341_DC_GPIO_Port  				LCD_DC_GPIO_Port
	#define ILI9341_TOUCH_CS_Pin				TOUCH_CS_Pin
	#define ILI9341_TOUCH_CS_GPIO_Port			TOUCH_CS_GPIO_Port
	#define ILI9341_TOUCH_IRQ_Pin				TOUCH_IRQ_Pin
	#define ILI9341_TOUCH_IRQ_GPIO_Port			TOUCH_IRQ_GPIO_Port
#endif // #ifdef LCD_2I8_ENABLED

#ifdef ENCODER_ENABLED
	#define ENCODER_BTN_Port					ENCODER1_BUTTON_GPIO_Port
	#define ENCODER_BTN_Pin						ENCODER1_BTN_Pin

	#define ENCODER_CLOCK_Port					ENCODER1_PHASE_GPIO_Port
	#define ENCODER_CLOCK_Pin					ENCODER1_PHASE_Pin

	#define ENCODER_DATA_Port					ENCODER1_DATA_GPIO_Port
	#define ENCODER_DATA_Pin					ENCODER1_DATA_Pin
#endif // #ifdef ENCODER_ENABLED

#ifdef CODEC_ENABLED
	#define AUDIO_BUF_SIZE 			256
	#ifdef CODEC_NAU88C22
		#define SYNTH_ENGINE_ENABLE	1
		#define NAU88C22_ADDR		(0x1a << 1)
		#define NAU88C22_TIMEOUT	(1000)
		extern	I2C_HandleTypeDef 	hi2c1;
		#define	NAU88C22_I2C		hi2c1
		extern	I2S_HandleTypeDef 	hi2s2;
		#define	NAU88C22_I2S		hi2s2
		#define	EXT_FREQ_12_288MHZ	1
	#endif // #ifdef CODEC_NAU88C22
#endif // #ifdef CODEC_ENABLED

#define	SAMPLE_FREQUENCY				48000
#define	SAMPLE_PERIOD					(1.0F / SAMPLE_FREQUENCY)

#define DMA_NOCACHE_RAM		__attribute__((section(".dmaNoCache")))  __attribute__ ((aligned (32)))
#define OSCILLATORS_RAM		__attribute__((section(".oscillators"))) __attribute__ ((aligned (32)))
#define AUDIOBUFS_RAM		__attribute__((section(".audiobufs")))   __attribute__ ((aligned (32)))
#define	ITCM_AREA_CODE		__attribute__((section(".RamITCMFunc"))) __attribute__ ((aligned (32)))
#define DTCM_VECTORS_DATA	__attribute__((section(".dtcm_data")))   __attribute__ ((aligned (32)))
#define FRAME_BUFFER		__attribute__((section(".framebuffer"))) __attribute__ ((aligned (32)))
#define D2_BUFFER			__attribute__((section(".d2ram"))) 		 __attribute__ ((aligned (32)))

#define FLASH_DATA_TABLE	__attribute__((section(".table"))) 		 __attribute__ ((aligned (32))) const

#define	BOARD_NAME			"VB1xx"
#define	MACHINE_NAME		"Vb101_VCO"
#define	MACHINE_VERSION		"A"


#endif /* BOARDS_VB1XX_H_ */
