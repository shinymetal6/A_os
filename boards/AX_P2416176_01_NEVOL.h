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
 * AX_P2416176_01_NEVOL.h
 *
 *  Created on: May 23, 2024
 *      Author: fil
 */

#ifndef BOARDS_AX_P2416176_01_NEVOL_H_
#define BOARDS_AX_P2416176_01_NEVOL_H_

/* Memories */

// memory pool defines
extern	uint8_t					*_mempool_start,*_mempool_end;
#define	POOL_START			    (uint32_t )(&_mempool_start)
#define	POOL_END			    (uint32_t )(&_mempool_end)
#define	POOL_CHUNK_SIZE		    256
#define	POOL_SIZE			    (POOL_END - POOL_START)
// POOL_NUM must be a constant value to compile
#define	POOL_NUM			    32
//#define POOL_ENABLE				1

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

#define	LED_1_GPIOPORT			LED_GPIO_Port
#define	LED_1_GPIOBIT			LED_Pin

//#define	QSPI_ENABLED			1
//#define	WIRELESS_ENABLED		1
//#define	WIRELESS_NRF24L01		1
//#define	LORA_ENABLED			1
//#define	NETWORKING_ENABLED		1
#define	USB_DEVICE_ENABLED				1
//#define	XMODEM_ENABLE			1
//#define	USB_XMODEM_ENABLE		1
//#define	MODBUS_ENABLE			1
//#define SENSORS_CCS8118			1
//#define MQTT_ENABLE				1
#define	POWERSAVING_ENABLED			1
#define	INTEL_HEX_DECODER_ENABLE	1

#ifdef NETWORKING_ENABLED
	#define	WIFI_ESP01S				1
	//#undef 	LWIP_DHCP
#endif // #ifdef NETWORKING_ENABLED

#ifdef USB_DEVICE_ENABLED
	#define	USB_CDC				1
	//#define	USB_MIDI			1
#endif // #ifdef USB_ENABLED

#ifdef QSPI_ENABLED
	#define	QSPI_WINBOND			1
	extern	QSPI_HandleTypeDef 	hqspi;
	#define	HQSPI1				hqspi
#endif // #ifdef QSPI_ENABLED
#ifdef WIRELESS_ENABLED
	#ifdef WIRELESS_NRF24L01
		extern	SPI_HandleTypeDef 					hspi1;
		#define NRF24L01_SPI						hspi1
		#define NRF24L01_SPI_TIMEOUT				2000

		#define NRF24L01_SS_PIN_PORT				ISM_SS_GPIO_Port
		#define NRF24L01_SS_PIN_NUMBER				ISM_SS_Pin

		#define NRF24L01_CE_PIN_PORT				ISM_CE_GPIO_Port
		#define NRF24L01_CE_PIN_NUMBER				ISM_CE_Pin

		#define NRF24L01_IRQ_PIN_PORT				ISM_IRQ_GPIO_Port
		#define NRF24L01_IRQ_PIN_NUMBER				ISM_IRQ_Pin
	#endif // #ifdef WIRELESS_NRF24L01
	#ifdef WIFI_ESP01S
		extern	UART_HandleTypeDef 					huart2;
		#define WIFI_ESP01S_UART					huart2
		#define	WIFI_RESET_GPIOPORT					WIFI_RESET_N_GPIO_Port
		#define	WIFI_RESET_GPIOBIT					WIFI_RESET_N_Pin
	#endif // #ifdef WIFI_ESP01S
	#ifdef LORA_ENABLED
		extern	SPI_HandleTypeDef 					hspi1;
		#define SX126X_SPI							hspi1
		#define SX126X_SPI_TIMEOUT					2000

		#define SX126X_SS_PIN_PORT					SX126x_SS_GPIO_Port
		#define SX126X_SS_PIN_NUMBER				SX126x_SS_Pin

		#define SX126X_BUSY_PIN_PORT				SX126x_BUSY_GPIO_Port
		#define SX126X_BUSY_PIN_NUMBER				SX126x_BUSY_Pin

		#define SX126X_DIO1_PIN_PORT				SX126x_DIO1_GPIO_Port
		#define SX126X_DIO1_PIN_NUMBER				SX126x_DIO1_Pin

		#define SX126X_RESET_GPIO_Port				SX126x_RESET_GPIO_Port
		#define SX126X_RESET_Pin					SX126x_RESET_Pin
	#endif // #ifdef LORA_ENABLED
#endif // #ifdef WIRELESS_ENABLED

#define	INTERNAL_ADC_ENABLED				1
#ifdef INTERNAL_ADC_ENABLED
	extern	ADC_HandleTypeDef 	hadc1;
	#define	ADC_HANDLE			hadc1
	#define	ADC_CHANNEL			3
	extern	TIM_HandleTypeDef 	htim6;
	#define	ADC_TIMER			htim6
	#define	ADC_SINGLE_CHANNEL	1
	#ifdef ADC_SINGLE_CHANNEL
		#define	ADC_SINGLE_CHANNEL_NUMBER	0
	#endif // #ifdef ADC_SINGLE_CHANNEL
#endif // #ifdef INTERNAL_ADC_ENABLED

#define	ITCM_AREA_CODE		__attribute__((section(".RamITCMFunc"))) __attribute__ ((aligned (32)))
#define DTCM_VECTORS_DATA	__attribute__((section(".dtcm_data")))   __attribute__ ((aligned (32)))
#define DWNLD_DATA_AREA		__attribute__((section(".d2ram")))   	 __attribute__ ((aligned (32)))
#define ETH_DATA_AREA		__attribute__((section(".d2ram")))   	 __attribute__ ((aligned (32)))

//#define	A_HAS_UART1			1
#define	A_HAS_UART2			1
#define	A_HAS_UART3				1
//#define	A_HAS_UART4			1
#define	A_HAS_UART5			1
//#define	A_HAS_UART6			1
//#define	A_HAS_UART7			1
//#define	A_HAS_UART8			1
#if defined(A_HAS_UART1) || defined(A_HAS_UART2) || defined(A_HAS_UART3) || defined(A_HAS_UART4) || defined(A_HAS_UART5) || defined(A_HAS_UART6) || defined(A_HAS_UART7) || defined(A_HAS_UART8)
	#define	A_HAS_UARTS			1
#endif

//#define	A_HAS_SPI1				1
//#define	A_HAS_SPI2				1
//#define	A_HAS_SPI3				1
//#define	A_HAS_SPI4				1
#if defined(A_HAS_SPI1) || defined(A_HAS_SPI2) || defined(A_HAS_SPI3) || defined(A_HAS_SPI4)
	#define	A_HAS_SPI_BUS				1
#endif

#define	A_HAS_I2C1				1
//#define	A_HAS_I2C2				1
//#define	A_HAS_I2C3				1
#define	A_HAS_I2C4				1
#if defined(A_HAS_I2C1) || defined(A_HAS_I2C2) || defined(A_HAS_I2C3) || defined(A_HAS_I2C4)
	#define	A_HAS_I2C_BUS				1
#endif

#ifdef	A_HAS_I2C_BUS
	#define	I2CFLASH_ENABLED		1
	#ifdef I2CFLASH_ENABLED
		#define	I2CFLASH_ADDRESS	0xa0
		#define	I2CFLASH_DEVICE0	I2CFLASH_ADDRESS
		#define	I2CFLASH_DEVICE1	I2CFLASH_ADDRESS+0x02
		#define	I2CFLASH_DEVICE2	I2CFLASH_ADDRESS+0x04
		#define	I2CFLASH_DEVICE3	I2CFLASH_ADDRESS+0x06
		#define	I2CFLASH_DEVICE4	I2CFLASH_ADDRESS+0x08
		#define	I2CFLASH_DEVICE5	I2CFLASH_ADDRESS+0x0a
		#define	I2CFLASH_DEVICE6	I2CFLASH_ADDRESS+0x0c
		#define	I2CFLASH_DEVICE7	I2CFLASH_ADDRESS+0x0e
	#endif // #ifdef I2CFLASH_ENABLED
#endif // #ifdef A_HAS_I2C_BUS

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


/* Clock */
#define TICK_HZ 				1000U
#define HSI_CLOCK         		480000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK
/* Others */
#define	PendSV_PRIORITY			15
#define	SysTick_PRIORITY		12
#define	ASSIGNED				1


#define	DFU_BOOT_ENTRY			0x1FF09800
#define	DFU_BOOT_VERSION_PTR	0x1FF1E7FE
#define	DFU_BOOT_VERSION		0x91

#define	BOARD_NAME			"NEVOL-2416176-01"
#define	MACHINE_NAME		"Nevol"
#define	MACHINE_VERSION		"A"


#endif /* BOARDS_AX_P2416176_01_NEVOL_H_ */
