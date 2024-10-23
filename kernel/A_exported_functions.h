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
 * A_exported_functions.h
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */

#ifndef KERNEL_A_EXPORTED_FUNCTIONS_H_
#define KERNEL_A_EXPORTED_FUNCTIONS_H_

#include "main.h"
#include "system_default.h"

/* System */
extern	void wait_event(uint32_t events);

extern	uint8_t get_current_process(void);
/*
extern	uint32_t get_activation_flags(void);
extern	uint32_t get_wakeup_rsn(void);
*/
extern	uint32_t get_wakeup_flags(uint32_t *reason, uint32_t *flags );

extern	void A_TimeDebug_High(void);
extern	void A_TimeDebug_Low(void);

/* Memory */
extern	uint8_t *mem_get(uint32_t size );
extern	uint32_t mem_release(uint8_t *data_ptr);

/* timer */
extern	void DWT_Delay_us(uint32_t au32_microseconds);
extern	void task_delay(uint32_t tick_count);
extern	uint32_t create_timer(uint8_t timer_id,uint32_t tick_count,uint8_t flags);
extern	uint32_t start_timer(uint8_t timer_id);
extern	uint32_t restart_timer(uint8_t timer_id,uint32_t tick_count,uint8_t flags);
extern	uint32_t stop_timer(uint8_t timer_id);
extern	uint32_t destroy_timer(uint8_t timer_id);
extern	uint8_t get_timer_expired(void);
extern	int32_t A_GetTick(void);
extern	uint32_t set_before_check_timers_callback(void (*callback)(void));
extern	uint32_t unset_before_check_timers_callback(void (*callback)(void));
extern	uint32_t set_after_check_timers_callback(void (*callback)(void));
extern	uint32_t unset_after_check_timers_callback(void (*callback)(void));

/* qspi */

/* flash */
extern	uint32_t get_flash_storage_ptr(void);
extern 	uint32_t get_flash_size(void);

/* compatibility */
extern	void 	A_Error_Handler(char * file, int line);

/* timer id */
#define	TIMER_ID_0		0x01
#define	TIMER_ID_1		0x02
#define	TIMER_ID_2		0x04
#define	TIMER_ID_3		0x08
#define	TIMER_ID_4		0x10
#define	TIMER_ID_5		0x20
#define	TIMER_ID_6		0x40
#define	TIMER_ID_7		0x80

/* timer flags */
#define	TIMERFLAGS_ONESHOT			0x00
#define	TIMERFLAGS_FOREVER			0x01
#define	TIMERFLAGS_ENABLED			0x02
#define	TIMERFLAGS_IN_USE			0x80
#define	TIMERFLAGS_DISABLED			0x00
#define	TIMERFLAGS_USERMASK			0x0f

#include "system_functions.h"

/* hwmanager */
extern	uint32_t allocate_hw(uint8_t peripheral,uint8_t config);
extern	uint32_t deallocate_hw(uint8_t peripheral);
extern	uint32_t allocate_hw_with_irq_callback(uint8_t bus_peripheral,uint8_t device_peripheral,uint8_t config,void (*irq_callback)(void));
extern	uint8_t get_busdevice_from_device(uint8_t device_peripheral);

/* hwmanager : usb */
extern	uint32_t hw_set_usb_rx_buffer(uint8_t *rx_buf);
extern	uint32_t hw_send_usb(uint8_t* ptr, uint16_t len);
extern	uint16_t hw_UsbGetRXLen(void);

/* hwmanager : uart */
extern	uint32_t hw_send_uart(uint8_t uart,uint8_t *ptr,uint16_t len);
extern	uint32_t hw_send_uart_dma(uint8_t uart,uint8_t *ptr,uint16_t len);
extern	uint32_t hw_receive_uart(uint8_t uart,uint8_t *rx_buf,uint16_t rx_buf_max_len,uint16_t timeout);
extern	uint32_t hw_receive_uart_sentinel(uint8_t uart,uint8_t *rx_buf,uint16_t rx_buf_max_len,uint8_t sentinel_start, uint8_t sentinel_end,uint16_t timeout);
extern	uint32_t hw_receive_uart_sentinel_clear(uint8_t uart);
extern	void HAL_UART_RxTimeoutCheckCallback(void);
extern	uint16_t hw_get_uart_receive_len(uint8_t uart);

/* module_manager */
extern	uint32_t allocate_module(uint32_t module,uint8_t config);
extern	uint32_t deallocate_module(uint32_t module);

/* mailbox */
extern void mbx_send(uint8_t process_number,uint8_t mailbox_number,uint8_t *mbx_ptr,uint32_t mbx_size);
extern	uint32_t mbx_receive(uint8_t mailbox_number,uint8_t *buf_ptr);

/* support functions */
extern	uint32_t A_bit_index_to_num(uint32_t bit_index );

/* audio */

#include "audio.h"

extern	uint32_t *InitAudioBuffers(void);
extern	uint8_t StartAudioBuffers(int16_t* audio_in_buffer,int16_t* audio_out_buffer);
extern	void SetEffectMode(void);
extern	void SetGeneratorMode(void);
extern	void Vca( WaveLR_t *buffer_out,WaveLR_t *buffer_in,uint16_t	start);
extern	void SetMasterVolume(uint16_t volume);
extern	void EnableOscillator(uint16_t channel, uint16_t midi_note , uint8_t velocity);
extern	void InitOscillators(void);
extern	void DisableOscillator(uint16_t channel, uint16_t midi_note , uint8_t velocity);
extern	void EnableOscillator(uint16_t channel, uint16_t midi_note , uint8_t velocity);
/* svc ops */
extern	int32_t call_svc(int8_t svc_index,int32_t param1 , int32_t param2 , int32_t param3);


#include "../drivers/gpio_utls/gpio.h"
#include "../drivers/drivers_manager.h"

#ifdef DCC_SYSTEM_ENABLE
#include "../drivers/actuators/dcc/dcc.h"
#endif

#ifdef INTERNAL_ADC_DRIVER
#include "../drivers/analog/intadc_driver/intadc_driver.h"
#endif

#ifdef INTERNAL_DAC_DRIVER
#include "../drivers/analog/intdac_driver/intdac_driver.h"
#endif

#ifdef INTERNAL_ADC_ENABLED
#include "../drivers/internal_adc/internal_adc.h"
#endif

#ifdef INTERNAL_DAC_ENABLED
#include "../drivers/internal_dac/internal_dac.h"
#endif

#ifdef XMODEM_ENABLE
#include "../modules/serial_transfers/xmodem.h"
#endif

#ifdef USB_XMODEM_ENABLE
#include "../modules/usb_transfer/usb_xmodem.h"
#endif

#ifdef INTEL_HEX_DECODER_ENABLE
#include "../modules/hex_decoders/hex_decoders_common.h"
#include "../modules/hex_decoders/ihex.h"
#endif

#ifdef MOTOROLA_HEX_DECODER_ENABLE
#include "../modules/hex_decoders/hex_decoders_common.h"
#include "../modules/hex_decoders/s3_hex.h"
#endif

#ifdef MODBUS_ENABLE
#include "../modules/modbus/modbus.h"
#endif

#ifdef WIRELESS_NRF24L01
#include "../drivers/wireless/nrf24l01/nrf24l01.h"
#endif

#ifdef CODEC_ENABLED
	#include "audio.h"
	#ifdef CODEC_NAU88C22
		#include "../drivers/codec/nau88c22.h"
	#endif
#endif // #ifdef CODEC_ENABLED

#ifdef LCD_096_ENABLED
	#include "../drivers/lcd_st7735/st7735.h"
	#include "../drivers/lcd_st7735/lcd_7735.h"
#endif // #ifdef LCD_096_ENABLED

#ifdef LCD_2I8_ENABLED
	#include "../drivers/lcd_ili9341/lcd_ili9341.h"
#endif // #ifdef LCD_2I8_ENABLED

#ifdef ADC_ENABLED
	#include "../drivers/internal_adc/internal_adc.h"
#endif // #ifdef ADC_ENABLED

#ifdef QSPI_ENABLED
	#include "HwDevices/hw_qspi.h"
	#include "../drivers/external_flash/qspi.h"
	#include "../drivers/external_flash/w25q.h"
#endif

#ifdef I2CFLASH_ENABLED
	#include "../drivers/external_flash/i2c_24aaxxx.h"
#endif // #ifdef I2CFLASH_ENABLED

#ifdef INTERNAL_ADC_ENABLED
	#include "../drivers/internal_adc/internal_adc.h"
#endif // #ifdef INTERNAL_ADC_ENABLED

#ifdef INTERNAL_DAC_ENABLED
	#include "../drivers/internal_dac/internal_dac.h"
#endif // #ifdef INTERNAL_DAC_ENABLED

#ifdef INTERNAL_RTC_ENABLED
	#include "../drivers/internal_rtc/internal_rtc.h"
#endif // #ifdef INTERNAL_RTC_ENABLED

#ifdef SYNTH_ENGINE_ENABLE
	#include "../modules/audio/effects.h"
#endif // #ifdef SYNTH_ENGINE_ENABLE

#ifdef DMX512_ENABLE
	#include "../modules/dmx512/dmx512.h"
#endif // #ifdef DMX512_ENABLE

#ifdef NETWORKING_ENABLED
#include "../modules/lwip2.2/App/lwip.h"
#ifdef MQTT_ENABLE
	#include "../modules/mqtt_client/mqtt_client.h"
#endif // #ifdef MQTT_ENABLE
#ifdef WIFI_ESP01S
	#include "../modules/WiFi/esp01_s.h"
#endif // 	#ifdef WIFI_ESP01S
#endif // #ifdef NETWORKING_ENABLED

#include "dfu_manager.h"
#include "flash_updater/flash_updater_common.h"

/* devices , maximum index is 31 */
#define	HWDEV_SPILCD				0
#define	HWDEV_NRF24L01				1
#define	HWDEV_WIFI_ESP01S			2

/* peripherals , index 31 is for modules sw irq */
#define	HW_SLEEP_FOREVER			0
#define	HW_DELAY					1
#define	HW_TIMER					2
#define	HW_MBX						3
#define	HW_UART1					4
#define	HW_UART2					5
#define	HW_UART3					6
#define	HW_UART4					7
#define	HW_UART5					8
#define	HW_UART6					9
#define	HW_UART7					10
#define	HW_UART8					11
#define	HW_I2C1						12
#define	HW_I2C2						13
#define	HW_I2C3						14
#define	HW_I2C4						15
#define	HW_SPI1						16
#define	HW_SPI2						17
#define	HW_SPI3						18
#define	HW_SPI4						19
#define	HW_QSPI						20
#define	HW_I2S1						21
#define	HW_I2S2						22
#define	HW_TIM						23
#define	HW_EXT_INT					24
#define	HW_ADC1						25
#define	HW_ADC2						26
#define	HW_DAC						27
#define	HW_USB_DEVICE				28
#define	HW_USB_HOST					29
#define	SW_MODULES					31

/* event to wait */
#define	EVENT_DELAY						(1<<HW_DELAY)
#define	EVENT_TIMER						(1<<HW_TIMER)
#define	EVENT_MBX						(1<<HW_MBX)
#define	EVENT_UART1_IRQ					(1<<HW_UART1)
#define	EVENT_UART2_IRQ					(1<<HW_UART2)
#define	EVENT_UART3_IRQ					(1<<HW_UART3)
#define	EVENT_UART4_IRQ					(1<<HW_UART4)
#define	EVENT_UART5_IRQ					(1<<HW_UART5)
#define	EVENT_UART6_IRQ					(1<<HW_UART6)
#define	EVENT_UART7_IRQ					(1<<HW_UART7)
#define	EVENT_UART8_IRQ					(1<<HW_UART8)
#define	EVENT_I2C1_IRQ					(1<<HW_I2C1)
#define	EVENT_I2C2_IRQ					(1<<HW_I2C2)
#define	EVENT_I2C3_IRQ					(1<<HW_I2C3)
#define	EVENT_I2C4_IRQ					(1<<HW_I2C4)
#define	EVENT_SPI1_IRQ					(1<<HW_SPI1)
#define	EVENT_SPI2_IRQ					(1<<HW_SPI2)
#define	EVENT_SPI3_IRQ					(1<<HW_SPI3)
#define	EVENT_SPI4_IRQ					(1<<HW_SPI4)
#define	EVENT_QSPI_IRQ					(1<<HW_QSPI)
#define	EVENT_I2S1_IRQ					(1<<HW_I2S1)
#define	EVENT_I2S2_IRQ					(1<<HW_I2S2)
#define	EVENT_TIM_IRQ					(1<<HW_TIM)
#define	EVENT_EXT_INT_IRQ				(1<<HW_EXT_INT)
#define	EVENT_ADC1_IRQ					(1<<HW_ADC1)
#define	EVENT_ADC2_IRQ					(1<<HW_ADC2)
#define	EVENT_DAC_IRQ					(1<<HW_DAC)
#define	EVENT_USB_DEVICE_IRQ			(1<<HW_USB_DEVICE)
#define	EVENT_USB_IRQ					(1<<HW_USB_HOST)
#define	EVENT_SW_MODULES				(1<<SW_MODULES)
/* suspend_mode */
#define	SUSPEND_ON_DELAY				EVENT_DELAY
#define	SUSPEND_ON_TIMER				EVENT_TIMER
#define	SUSPEND_ON_MBX					EVENT_MBX
#define	SUSPEND_ON_UART1_IRQ			EVENT_UART1_IRQ
#define	SUSPEND_ON_UART2_IRQ			EVENT_UART2_IRQ
#define	SUSPEND_ON_UART3_IRQ			EVENT_UART3_IRQ
#define	SUSPEND_ON_UART4_IRQ			EVENT_UART4_IRQ
#define	SUSPEND_ON_UART5_IRQ			EVENT_UART5_IRQ
#define	SUSPEND_ON_UART6_IRQ			EVENT_UART6_IRQ
#define	SUSPEND_ON_UART7_IRQ			EVENT_UART7_IRQ
#define	SUSPEND_ON_UART8_IRQ			EVENT_UART8_IRQ
#define	SUSPEND_ON_I2C1_IRQ				EVENT_I2C1_IRQ
#define	SUSPEND_ON_I2C2_IRQ				EVENT_I2C2_IRQ
#define	SUSPEND_ON_I2C3_IRQ				EVENT_I2C3_IRQ
#define	SUSPEND_ON_I2C4_IRQ				EVENT_I2C4_IRQ
#define	SUSPEND_ON_SPI1_IRQ				EVENT_SPI1_IRQ
#define	SUSPEND_ON_SPI2_IRQ				EVENT_SPI2_IRQ
#define	SUSPEND_ON_SPI3_IRQ				EVENT_SPI3_IRQ
#define	SUSPEND_ON_SPI4_IRQ				EVENT_SPI4_IRQ
#define	SUSPEND_ON_QSPI_IRQ				EVENT_QSPI_IRQ
#define	SUSPEND_ON_I2S1_IRQ				EVENT_I2S1_IRQ
#define	SUSPEND_ON_I2S2_IRQ				EVENT_I2S2_IRQ
#define	SUSPEND_ON_TIM_IRQ				EVENT_TIM_IRQ
#define	SUSPEND_ON_EXT_INT_IRQ			EVENT_EXT_INT_IRQ
#define	SUSPEND_ON_ADC1_IRQ				EVENT_ADC1_IRQ
#define	SUSPEND_ON_ADC2_IRQ				EVENT_ADC2_IRQ
#define	SUSPEND_ON_DAC_IRQ				EVENT_DAC_IRQ
#define	SUSPEND_ON_USB_DEVICE_IRQ		EVENT_USB_DEVICE_IRQ
#define	SUSPEND_ON_USB_HOST_IRQ			EVENT_USB_HOST_IRQ
#define	SUSPEND_ON_SW_MODULES_IRQ		EVENT_SW_MODULES
/* wakeup_flags */
#define	WAKEUP_FROM_DELAY				SUSPEND_ON_DELAY
#define	WAKEUP_FROM_TIMER				SUSPEND_ON_TIMER
#define	WAKEUP_FROM_MBX					SUSPEND_ON_MBX
#define	WAKEUP_FROM_UART1_IRQ			SUSPEND_ON_UART1_IRQ
#define	WAKEUP_FROM_UART2_IRQ			SUSPEND_ON_UART2_IRQ
#define	WAKEUP_FROM_UART3_IRQ			SUSPEND_ON_UART3_IRQ
#define	WAKEUP_FROM_UART4_IRQ			SUSPEND_ON_UART4_IRQ
#define	WAKEUP_FROM_UART5_IRQ			SUSPEND_ON_UART5_IRQ
#define	WAKEUP_FROM_UART6_IRQ			SUSPEND_ON_UART6_IRQ
#define	WAKEUP_FROM_UART7_IRQ			SUSPEND_ON_UART7_IRQ
#define	WAKEUP_FROM_UART8_IRQ			SUSPEND_ON_UART8_IRQ
#define	WAKEUP_FROM_I2C1_IRQ			SUSPEND_ON_I2C1_IRQ
#define	WAKEUP_FROM_I2C2_IRQ			SUSPEND_ON_I2C2_IRQ
#define	WAKEUP_FROM_I2C3_IRQ			SUSPEND_ON_I2C3_IRQ
#define	WAKEUP_FROM_I2C4_IRQ			SUSPEND_ON_I2C4_IRQ
#define	WAKEUP_FROM_SPI1_IRQ			SUSPEND_ON_SPI1_IRQ
#define	WAKEUP_FROM_SPI2_IRQ			SUSPEND_ON_SPI2_IRQ
#define	WAKEUP_FROM_SPI3_IRQ			SUSPEND_ON_SPI3_IRQ
#define	WAKEUP_FROM_SPI4_IRQ			SUSPEND_ON_SPI4_IRQ
#define	WAKEUP_FROM_QSPI_IRQ			SUSPEND_ON_QSPI_IRQ
#define	WAKEUP_FROM_I2S1_IRQ			SUSPEND_ON_I2S1_IRQ
#define	WAKEUP_FROM_I2S2_IRQ			SUSPEND_ON_I2S2_IRQ
#define	WAKEUP_FROM_TIM_IRQ				SUSPEND_ON_TIM_IRQ
#define	WAKEUP_FROM_EXT_INT_IRQ			SUSPEND_ON_EXT_INT_IRQ
#define	WAKEUP_FROM_ADC1_IRQ			SUSPEND_ON_ADC1_IRQ
#define	WAKEUP_FROM_ADC2_IRQ			SUSPEND_ON_ADC2_IRQ
#define	WAKEUP_FROM_DAC_IRQ				SUSPEND_ON_DAC_IRQ
#define	WAKEUP_FROM_USB_DEVICE_IRQ		SUSPEND_ON_USB_DEVICE_IRQ
#define	WAKEUP_FROM_USB_HOST_IRQ		SUSPEND_ON_USB_HOST_IRQ
#define	WAKEUP_FROM_SW_MODULES_IRQ		SUSPEND_ON_SW_MODULES_IRQ
/* device_flags */
#define	DEVICE_DELAY					HW_DELAY
#define	DEVICE_TIMER					HW_TIMER
#define	DEVICE_MBX						HW_MBX
#define	DEVICE_UART1					HW_UART1
#define	DEVICE_UART2					HW_UART2
#define	DEVICE_UART3					HW_UART3
#define	DEVICE_UART4					HW_UART4
#define	DEVICE_UART5					HW_UART5
#define	DEVICE_UART6					HW_UART6
#define	DEVICE_UART7					HW_UART7
#define	DEVICE_UART8					HW_UART8
#define	DEVICE_I2C1						HW_I2C1
#define	DEVICE_I2C2						HW_I2C2
#define	DEVICE_I2C3						HW_I2C3
#define	DEVICE_I2C4						HW_I2C4
#define	DEVICE_SPI1						HW_SPI1
#define	DEVICE_SPI2						HW_SPI2
#define	DEVICE_SPI3						HW_SPI3
#define	DEVICE_SPI4						HW_SPI4
#define	DEVICE_QSPI						HW_QSPI
#define	DEVICE_I2S1						HW_I2S1
#define	DEVICE_I2S2						HW_I2S2
#define	DEVICE_TIM						HW_TIM
#define	DEVICE_EXT_INT					HW_EXT_INT
#define	DEVICE_ADC1						HW_ADC1
#define	DEVICE_ADC2						HW_ADC2
#define	DEVICE_DAC						HW_DAC
#define	DEVICE_USB_DEVICE				HW_USB_DEVICE
#define	DEVICE_USB_HOST					HW_USB_HOST
#define	DEVICE_SW_MODULES_IRQ			WAKEUP_FROM_SW_MODULES_IRQ

/* wakeup_flags */
#define	WAKEUP_FLAGS_TIM_ID0				0x00000001
#define	WAKEUP_FLAGS_TIM_ID1				0x00000002
#define	WAKEUP_FLAGS_TIM_ID2				0x00000004
#define	WAKEUP_FLAGS_TIM_ID3				0x00000008
#define	WAKEUP_FLAGS_TIM_ID4				0x00000010
#define	WAKEUP_FLAGS_TIM_ID5				0x00000020
#define	WAKEUP_FLAGS_TIM_ID6				0x00000040
#define	WAKEUP_FLAGS_TIM_ID7				0x00000080
#define	WAKEUP_FLAGS_UART_RX				0x00000100
#define	WAKEUP_FLAGS_UART_TX				0x00000200
#define	WAKEUP_FLAGS_UART_TO				0x00000400
#define	WAKEUP_FLAGS_SPI_TX					0x00000800
#define	WAKEUP_FLAGS_SPI_RX					0x00001000
#define	WAKEUP_FLAGS_HALF_DMA				0x00002000
#define	WAKEUP_FLAGS_FULL_DMA				0x00004000
#define	WAKEUP_FLAGS_SPILCD					0x01000000
#define	WAKEUP_FLAGS_EXTI_D0				0x02000000
#define	WAKEUP_FLAGS_EXTI_D1				0x04000000
#define	WAKEUP_FLAGS_EXTI					0x08000000
#define	WAKEUP_FLAGS_MBX					0x40000000
#define	WAKEUP_FLAGS_HW_USB_RX_COMPLETE		0x80000000

#define	HW_TIMERS_TIMER1					0
#define	HW_TIMERS_TIMER2					1
#define	HW_TIMERS_TIMER3					2
#define	HW_TIMERS_TIMER4					3
#define	HW_TIMERS_TIMER5					4
#define	HW_TIMERS_TIMER6					5
#define	HW_TIMERS_TIMER7					6
#define	HW_TIMERS_TIMER8					7
#define	HW_TIMERS_TIMER9					8
#define	HW_TIMERS_TIMER10					9
#define	HW_TIMERS_TIMER11					10
#define	HW_TIMERS_TIMER12					11
#define	HW_TIMERS_TIMER13					12
#define	HW_TIMERS_TIMER14					13
#define	HW_TIMERS_TIMER15					14
#define	HW_TIMERS_TIMER16					15
#define	HW_TIMERS_TIMER17					16


#define	MODULE_IDX_MQTT						0

//#define	MODULE_MQTT							(1 << MODULE_IDX_MQTT )
#define	MODULE_MQTT_RXFLAG					0x00000001
#define	MODULE_MQTT_TXFLAG					0x00000002

#endif /* KERNEL_A_EXPORTED_FUNCTIONS_H_ */
