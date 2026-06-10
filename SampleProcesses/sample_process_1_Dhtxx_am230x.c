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
 * sample_process_1_Dhtxx_am230x.c
 *
 *  Created on: Dec 4, 2024
 *      Author: fil
 */

#include "main.h"
#include "sample_A_os_includes.h"

#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef 	SAMPLEPROCESS_1_DHTxx

#define	DHT11_SENSOR_ID	0
extern	TIM_HandleTypeDef 		htim2;

extern	UART_HandleTypeDef hlpuart1;
Dhtxx_am230x_DriverStruct_t	Dhtxx_am230x_Drv =
{
		.sensor_id = DHT11_SENSOR_ID,
		.dht_timer = &htim2,
		.dht_timer_channel = TIM_CHANNEL_3,
		.one_wire_port = DHT_DATA_GPIO_Port,
		.one_wire_bit = DHT_DATA_Pin,
};
uint32_t		dht_driver_handle;
uint8_t dht_sm = 0;

#define	UART_WAKEUP			WAKEUP_FROM_UART1_IRQ
#define	UART_EVENT			EVENT_UART1_IRQ
#define	UART				hlpuart1
#define	UART_RX_BUF_SIZE	512
#define	UART_TX_BUF_SIZE	512
uint8_t	uart_rx_buffer[UART_RX_BUF_SIZE];
uint8_t	uart_tx_buffer[UART_TX_BUF_SIZE];

UART_DriverStruct_t Uart_Drv =
{
	.data = uart_rx_buffer,
	.rx_max_len = UART_RX_BUF_SIZE,
	.uart = &UART,
	.wakeup_id = UART_WAKEUP,
	.timeout = 1000,
	.flags = UART_USES_DMA_TX,
};
uint32_t	uart_driver_handle;

void sample_process_1_Dhtxx_am230x(uint32_t process_id)
{
uint32_t	wakeup,flags;
float humidity;
float temperature;
uint8_t negative;
	dht_driver_handle = dhtxx_am230x_register(&Dhtxx_am230x_Drv);
	dhtxx_am230x_init(dht_driver_handle);
	dhtxx_am230x_start(dht_driver_handle);
	uart_driver_handle = uart_register(&Uart_Drv);
	sprintf((char *)uart_tx_buffer,"DHT on Aos\n\r");
	uart_send(uart_driver_handle, uart_tx_buffer,strlen((char * )uart_tx_buffer));

	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	while(1)
	{
		wait_event(EVENT_TIMER);
		get_wakeup_flags(&wakeup,&flags);

		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			switch ( dht_sm)
			{
			case 0 :
			case 2 :
			case 3 :
			case 4 :
				dhtxx_am230x_start(dht_driver_handle);
				dht_sm++;
				break;
			case 8 :
				sprintf((char *)uart_tx_buffer,"Error on DHT Data\n\r");
				if ( dhtxx_am230x_get_status(dht_driver_handle) == 0 )
				{
					humidity = Dhtxx_am230x_Drv.dhtxx_data[0] + (Dhtxx_am230x_Drv.dhtxx_data[1] / 10.0);
					negative = 0;
					if ((Dhtxx_am230x_Drv.dhtxx_data[2] & 0x80) == 0x80 )
						negative = 1;

					uint16_t temp_abs = ((Dhtxx_am230x_Drv.dhtxx_data[2] & 0x7F) << 8) | Dhtxx_am230x_Drv.dhtxx_data[3];
					temperature = temp_abs / 10.0;
					if (negative)
						temperature = -temperature;
					sprintf((char *)uart_tx_buffer,"DHT Data : humidity = %f temperature = %f\n\r",humidity,temperature);
				}

				uart_send(uart_driver_handle, uart_tx_buffer,strlen((char * )uart_tx_buffer));
				dht_sm = 0;
				break;
			default :
				dht_sm++;
				break;
			}
			process_led();
		}
	}
}
#endif // #ifdef 	SAMPLEPROCESS_1_DHTxx

#endif // #ifdef SAMPLE_PROCESSES_ENABLED

