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
 * sample_process_1_lora_scanner.c
 *
 *  Created on: Jul 31, 2025
 *      Author: fil
 */

#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_LORA_SCANNER

#define	PRC1_TICK				10
extern	SPI_HandleTypeDef 		hspi1;

#define	LORA_BUFSIZE			128

uint8_t read_data[LORA_BUFSIZE];
uint8_t send_data[LORA_BUFSIZE];

extern	SPI_HandleTypeDef hspi1;

LORA_Drv_TypeDef	LORA_Drv =
{
	.wakeup_id = 1,
	.spi = &hspi1,
	.spi_timeout_ms = 100,
	.device_id = ID_SX1262,
	.CS_bit = LORA_SS_Pin,
	.CS_port = LORA_SS_GPIO_Port,
	.RESET_bit = LORA_RESET_Pin,
	.RESET_port = LORA_RESET_GPIO_Port,
	.IRQ_bit = LORA1_IRQPD1_Pin,
	.IRQ_port = LORA1_IRQPD1_GPIO_Port,
	.IRQ_number = EXTI0_IRQn,
	.RADIO_BUSY_bit = LORA_BUSY_Pin,
	.RADIO_BUSY_port = LORA_BUSY_GPIO_Port,
	.TX_Buf = send_data,
	.RX_Buf = read_data,
};

GPIO_Int_DriverStruct_t	LoRa_Int_Driver =
{
	.IRQ_bit = LORA1_IRQPD1_Pin,
	.IRQ_port = LORA1_IRQPD1_GPIO_Port,
	.IRQ_type = GPIO_INT_TYPE_RISING,
	.flags = GPIO_INT_WAKEUP_ON_EVENT,
	.wakeup_id = WAKEUP_FROM_EXT_INT_IRQ,
};

uint8_t	usb_rx_buffer[64];
uint8_t	usb_tx_buffer[64];

USB_Drv_TypeDef	Usb_channel =
{
		.requested_len = XMODEM_LINE_LEN,
		.data = usb_rx_buffer,
		.timeout = 10,
		.wakeup_id = WAKEUP_FROM_USB_DEVICE_IRQ,
};
uint32_t	usb_handle;

uint32_t	irq_cntr = 0;
uint32_t	lora_ok = 0;
/*
 * 433.175 to 434.665 with 250kHz width
 * 433177000
 */
#define	MINFREQ		433000000
#define	STEPFREQ	125000
#define	MAXFREQ		(MINFREQ+(8*STEPFREQ))

uint32_t	frequency = MINFREQ;
uint32_t	rssi;
uint32_t	banner = 0;
void sample_process_1_lora_scanner(uint32_t process_id)
{
uint32_t	wakeup,flags;

	if ( LoRa_register(&LORA_Drv) == 0 )
		lora_ok = 1;
	if ( lora_ok == 1 )
	{
		LoRa_Init();
		// link the gpio irq to lora driver
		LoRa_Int_Driver.irq_exti_callback = LORA_Drv.LoRa_HandleCallback,
		// then register the driver
		gpio_int_register(&LoRa_Int_Driver);
	}
	LoRa_SetFrequency(frequency);
	LoRa_SetModeReceive();

	usb_handle = usb_device_driver_register(&Usb_channel);

	create_timer(TIMER_ID_0,PRC1_TICK,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	create_timer(TIMER_ID_1,PRC1_TICK*800,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	while(1)
	{
		wait_event(EVENT_TIMER | EVENT_EXT_INT_IRQ);
		get_wakeup_flags(&wakeup,&flags);

		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			if (( flags & TIMER_ID_0) == TIMER_ID_0)
			{
				process_led();
				if ( banner <20 )
					banner++;
				if ( banner == 20 )
				{
					sprintf((char *)usb_rx_buffer,"LoRa Scanner\n\r");
					usb_send(usb_handle,usb_rx_buffer,strlen((char *)usb_rx_buffer));
					banner = 21;
				}
			}
			if (( flags & TIMER_ID_1) == TIMER_ID_1)
			{
				rssi = LoRa_GetRSSI();
				sprintf((char *)usb_rx_buffer,"F : %d -> RSSI : %d\n\r",(int )frequency,(int )rssi);
				usb_send(usb_handle,usb_rx_buffer,strlen((char *)usb_rx_buffer));
				frequency += STEPFREQ;
				if ( frequency > MAXFREQ)
					frequency = MINFREQ;
				LoRa_SetFrequency(frequency);
			}
		}
		if (( wakeup & WAKEUP_FROM_EXT_INT_IRQ) == WAKEUP_FROM_EXT_INT_IRQ)
		{
			irq_cntr++;
		}
	}
}
#endif // #ifdef SAMPLEPROCESS_1_LORA_SCANNER
#endif // #ifdef SAMPLE_PROCESSES_ENABLED


