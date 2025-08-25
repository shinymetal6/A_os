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
 * sample_process_1_lora.c
 *
 *  Created on: Jun 26, 2025
 *      Author: fil
 */

/* Please note :
 * The pins DIO1 and DIO4 of the RA01S are programmed as input in the ioc.
 * No special functions ( irq ) are present, so they should be polled
 */

#include "main.h"
#include "A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_LORA

#define	PRC1_TICK				10

#define	LORA_BUFSIZE			256

uint8_t read_data[LORA_BUFSIZE];
uint8_t send_data[LORA_BUFSIZE];

int			RSSI;
#ifdef	STM32H753xx
	#ifdef STM32H753ZI_NUCLEO
		extern	SPI_HandleTypeDef hspi3;
		#define USE_SPI	hspi3
	#endif
#else
		extern	SPI_HandleTypeDef hspi1;
		#define USE_SPI	hspi1
#endif

LORA_Drv_TypeDef	LORA_Drv =
{
	.wakeup_id = 1,
	.spi = &USE_SPI,
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

uint32_t	irq_cntr = 0;
uint32_t	lora_ok = 0;

void sample_process_1_lora(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint32_t	tcounter=0;

	sprintf((char *)send_data,"This is LoRa packet from STM32\n\r");
	if ( LoRa_register(&LORA_Drv) == 0 )
		lora_ok = 1;
	if ( lora_ok == 1 )
	{
		LoRa_Init();
		// link the gpio irq to lora driver
		LoRa_Int_Driver.irq_exti_callback = LORA_Drv.LoRa_HandleCallback,
		// then register the driver
		gpio_int_register(&LoRa_Int_Driver);
		LoRaWAN_Init();
	}

	create_timer(TIMER_ID_0,PRC1_TICK,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	while(1)
	{
		wait_event(EVENT_TIMER | EVENT_EXT_INT_IRQ);
		get_wakeup_flags(&wakeup,&flags);

		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			if (( flags & TIMER_ID_0) == TIMER_ID_0)
			{
				process_led();
				tcounter++;
				if ( tcounter >= 120 )
				{
					tcounter = 0;
					if ( lora_ok == 1 )
						LoRa_Tx(send_data,strlen((char *)send_data));
				}
			}
		}
		if (( wakeup & WAKEUP_FROM_EXT_INT_IRQ) == WAKEUP_FROM_EXT_INT_IRQ)
		{
			irq_cntr++;
		}
	}
}
#endif // #ifdef SAMPLEPROCESS_1_LORA
#endif // #ifdef SAMPLE_PROCESSES_ENABLED

