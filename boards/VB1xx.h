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
#define	POOL_SIZE			    (POOL_END - POOL_START)
#define	POOL_CHUNK_SIZE		    256
// POOL_NUM must be a constant value to compile
#define	POOL_NUM			    (32)

// system defines
extern	uint8_t					*_osSysRam_start,*_osSysRam_end;
#define SRAM_START				(uint32_t )(&_osSysRam_start)
#define SRAM_END				(uint32_t )(&_osSysRam_end)
#define SRAM_SIZE				(&_osSysRam_end - &_osSysRam_start)
#define SIZE_SCHED_STACK		2048U
#define SCHED_STACK_START		SRAM_END

#define IDLE_STACK_START		(uint32_t )((uint32_t )&_osSysRam_end - (uint32_t )SIZE_SCHED_STACK)
#define SIZE_IDLE_STACK          2048U
#define	FIRST_PRC_STACK_START	 (IDLE_STACK_START - SIZE_IDLE_STACK)

/* I/O */

#define	LED_1_GPIOPORT			LED_GPIO_Port
#define	LED_1_GPIOBIT			LED_Pin

/* Clock */
#define TICK_HZ 				1000U
#define HSI_CLOCK         		480000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK


/* Chip peripherals utilization */
//#define	A_HAS_UART1			1
//#define	A_HAS_UART2			1
//#define	A_HAS_UART3				1
//#define	A_HAS_UART4			1
//#define	A_HAS_UART5			1
//#define	A_HAS_UART6			1
//#define	A_HAS_UART7			1
#if defined(A_HAS_UART1) || defined(A_HAS_UART2) || defined(A_HAS_UART3) || defined(A_HAS_UART4) || defined(A_HAS_UART5) || defined(A_HAS_UART6) || defined(A_HAS_UART7)
	#define	A_HAS_UARTS			1
#endif

#define	A_HAS_SPI1				1
//#define	A_HAS_SPI2				1
//#define	A_HAS_SPI3				1
//#define	A_HAS_SPI4				1
#if defined(A_HAS_SPI1) || defined(A_HAS_SPI2) || defined(A_HAS_SPI3) || defined(A_HAS_SPI4)
	#define	A_HAS_SPI_BUS				1
#endif

#define	A_HAS_I2C1				1
//#define	A_HAS_I2C2				1
//#define	A_HAS_I2C3				1
//#define	A_HAS_I2C4				1
#if defined(A_HAS_I2C1) || defined(A_HAS_I2C2) || defined(A_HAS_I2C3) || defined(A_HAS_I2C4)
	#define	A_HAS_I2C_BUS				1
#endif

//#define	A_HAS_TIMER1			1
//#define	A_HAS_TIMER2			1
//#define	A_HAS_TIMER3			1
//#define	A_HAS_TIMER4			1
//#define	A_HAS_TIMER5			1
//#define	A_HAS_TIMER6			1
//#define	A_HAS_TIMER7			1
//#define	A_HAS_TIMER8			1
//#define	A_HAS_TIMER9			1
//#define	A_HAS_TIMER10			1
//#define	A_HAS_TIMER11			1
//#define	A_HAS_TIMER12			1
//#define	A_HAS_TIMER13			1
//#define	A_HAS_TIMER14			1
//#define	A_HAS_TIMER15			1
//#define	A_HAS_TIMER16			1
#if defined	(A_HAS_TIMER1) || (A_HAS_TIMER2) || (A_HAS_TIMER3) || (A_HAS_TIMER4) || (A_HAS_TIMER5) || (A_HAS_TIMER6) \
				|| (A_HAS_TIMER7) || (A_HAS_TIMER8) || (A_HAS_TIMER9) || (A_HAS_TIMER10) || (A_HAS_TIMER11) || (A_HAS_TIMER12) \
				|| (A_HAS_TIMER13) || (A_HAS_TIMER14) || (A_HAS_TIMER15) || (A_HAS_TIMER16)
	#define	A_HAS_TIMERS				1
#endif

// Board configuration

#define	LED_GPIOPORT			LED_GPIO_Port
#define	LED_GPIOBIT				LED_Pin

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
#define	USB_DEVICE_ENABLED		1
//#define	USB_HOST_ENABLED		1

#define INTERNAL_ADC_ENABLED		1
#define INTERNAL_DAC_ENABLED		1

#ifdef INTERNAL_ADC_ENABLED
	extern	ADC_HandleTypeDef 			hadc1;
	#define ADC_HANDLE1					hadc1
	extern	ADC_HandleTypeDef 			hadc2;
	#define ADC_HANDLE2					hadc2
	extern	TIM_HandleTypeDef 			htim6;
	#define ADC1_TIMER					htim6
	#define	ADC2_USES_ADC1_TIMER			1
#ifdef ADC2_USES_ADC1_TIMER
	#define ADC2_TIMER					ADC1_TIMER
#else
	extern	TIM_HandleTypeDef 			htim7;
	#define ADC2_TIMER					htim7
#endif

	//#define ADC1_TIM_FOR_DAC				1
	//#define ADC2_TIM_FOR_DAC				1
	//#define ADC_HAS_OPAMP1				1
	#ifdef ADC_HAS_OPAMP1
		extern	OPAMP_HandleTypeDef 	hopamp1;
		#define OPAMP1_HANDLE			hopamp1
	#endif
	//#define ADC_HAS_OPAMP2				1
#ifdef ADC_HAS_OPAMP2
	extern	OPAMP_HandleTypeDef 	hopamp2;
	#define OPAMP_HANDLE			hopamp2
#endif
	//#define ADC_SINGLE_CHANNEL			1
	#ifdef ADC_SINGLE_CHANNEL
		#define ADC_SINGLE_CHANNEL_NUMBER	0
	#endif

#endif

#ifdef INTERNAL_DAC_ENABLED
	extern	DAC_HandleTypeDef 			hdac1;
	#define DAC_HANDLE					hdac1
	#define DAC_USER_WAVETABLE				1
	#ifdef DAC_USER_WAVETABLE
		#define DAC_WAVETABLE_SIZE				256
	#endif
	#ifdef ADC1_TIM_FOR_DAC
		#define DAC_TIMER				ADC1_TIMER
	#else
		#ifdef ADC2_TIM_FOR_DAC
			#define DAC_TIMER				ADC2_TIMER
		#else
			extern	TIM_HandleTypeDef 		htim15;
			#define DAC_TIMER				htim15
		#endif
	#endif
#endif

#define	USE_ITCM				1
#define	I2STX_PRIORITY			7
#define	I2SRX_PRIORITY			7

#ifdef USB_DEVICE_ENABLED
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

#ifdef LCD_096_ENABLED
	extern SPI_HandleTypeDef hspi1;
	#define ST7735_SPI_PORT 					hspi1
	#define LCD_HW_SPI_PORT 					HW_SPI1

	#define ST7735_SPI_TIMEOUT					2000

	#define ST7735_CS_GPIO_Port					LCD_SS_GPIO_Port
	#define ST7735_CS_Pin						LCD_SS_Pin

	#define ST7735_DC_GPIO_Port					LCD_DC_GPIO_Port
	#define ST7735_DC_Pin						LCD_DC_Pin

	#define ST7735_RES_GPIO_Port				LCD_RESET_GPIO_Port
	#define ST7735_RES_Pin						LCD_RESET_Pin
	#define LCD_ENABLED							1
	#define	SPILCD_USES_DMA						1
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
	#define LCD_ENABLED							1
	#define	SPILCD_USES_DMA						1
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
	#define AUDIO_BUF_SIZE 			1024
	#ifdef CODEC_NAU88C22
		#define SYNTH_ENGINE_ENABLE	1
		#define NAU88C22_ADDR		(0x1a << 1)
		#define NAU88C22_TIMEOUT	(1000)
		extern	I2C_HandleTypeDef 	hi2c1;
		#define	NAU88C22_I2C		hi2c1
		#define	NAU88C22_I2C_BUS	HW_I2C1

		extern	I2S_HandleTypeDef 	hi2s2;
		#define	NAU88C22_I2S		hi2s2
		#define	EXT_FREQ_12_288MHZ	1
	#endif // #ifdef CODEC_NAU88C22
#endif // #ifdef CODEC_ENABLED

#ifdef WIRELESS_ENABLED

#endif // #ifdef WIRELESS_ENABLED

#define	SAMPLE_FREQUENCY	48000
#define	SAMPLE_PERIOD		(1.0F / SAMPLE_FREQUENCY)

#define DMA_NOCACHE_RAM		__attribute__((section(".dmaNoCache")))  	__attribute__ ((aligned (32)))
#define AUDIO_FAST_RAM		__attribute__((section(".dtcm_user_data"))) __attribute__ ((aligned (32)))
#define AUDIOBUFS_RAM		__attribute__((section(".audiobufs")))  	__attribute__ ((aligned (32)))
#define	ITCM_AREA_CODE		__attribute__((section(".RamITCMFunc"))) 	__attribute__ ((aligned (32)))
#define DTCM_VECTORS_DATA	__attribute__((section(".dtcm_data")))   	__attribute__ ((aligned (32)))
#define FRAME_BUFFER		__attribute__((section(".framebuffer"))) 	__attribute__ ((aligned (32)))
#define D2_BUFFER			__attribute__((section(".d2ram"))) 		 	__attribute__ ((aligned (32)))

#define FLASH_DATA_TABLE	__attribute__((section(".table"))) 		 	__attribute__ ((aligned (32))) const


#define	DFU_BOOT_ENTRY			0x1FF09800
#define	DFU_BOOT_VERSION_PTR	0x1FF1E7FE
#define	DFU_BOOT_VERSION		0x91

#define	BOARD_NAME			"VB1xx"
#define	MACHINE_NAME		"Vb101_VCO_01"
#define	MACHINE_VERSION		"A"
#define	ASSIGNED			1


#endif /* BOARDS_VB1XX_H_ */
