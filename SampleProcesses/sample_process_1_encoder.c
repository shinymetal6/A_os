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
 * sample_process_1_encoder.c
 *
 *  Created on: Oct 6, 2025
 *      Author: fil
 */

#include "main.h"
#include "sample_A_os_includes.h"

#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef 	SAMPLEPROCESS_1_ENCODER
/*
 * for nucleo 743zi
 * cn10-6  : pe11 -> tim1 ch2
 * cn10-4  : pe9 -> tim1 ch1
 * */
extern	TIM_HandleTypeDef htim1;
extern	UART_HandleTypeDef huart3;

#define	UART_WAKEUP			WAKEUP_FROM_UART3_IRQ
#define	UART_EVENT			EVENT_UART3_IRQ
#define	UART				huart3

//#define	USE_CALLBACK	1

#ifdef USE_CALLBACK
extern	void irq_encoder_callback(uint32_t value);
extern	void irq_encoderbtn_callback(uint16_t value);
#endif // #ifdef USE_CALLBACK

Encoder_Drv_TypeDef	Encoder_Drv =
{
		//.encoder_timer = &htim1,
#ifdef USE_CALLBACK
		.irq_encoder_callback = irq_encoder_callback,
#else
		.wakeup_id = WAKEUP_FROM_TIM_IRQ,
#endif // #ifdef USE_CALLBACK
};
uint32_t encoder_driver_handle;

GPIO_Interrupt_DriverStruct_t Button_Driver =
{
	.IRQ_bit = ENC_BTN_Pin,
	.IRQ_port = ENC_BTN_GPIO_Port,
	.IRQ_type = GPIO_INT_TYPE_FALLING,
	.debounce = 100,
	.flags = GPIO_INT_WAKEUP_ON_EVENT,
#ifdef USE_CALLBACK
	.irq_exti_callback = irq_encoderbtn_callback,
#else
	.wakeup_id = WAKEUP_FROM_EXT_INT_IRQ,
#endif // #ifdef USE_CALLBACK
};
uint32_t button_driver_handle;

#define	UART_RX_BUF_SIZE	512
#define	UART_TX_BUF_SIZE	512
uint8_t	uart_rx_buffer[UART_RX_BUF_SIZE];
uint8_t	uart_tx_buffer[UART_TX_BUF_SIZE];

UART_Drv_TypeDef Uart_Drv =
{
	.data = uart_rx_buffer,
	.rx_max_len = UART_RX_BUF_SIZE,
	.uart = &UART,
	.wakeup_id = UART_WAKEUP,
	.timeout = 1000,
	.flags = UART_USES_DMA_TX,
};

uint32_t	uart_driver_handle;

#ifdef USE_CALLBACK
uint32_t	enc_val,btn_val,ready=0;;
void irq_encoder_callback(uint32_t value)
{
	enc_val = value;
}
void irq_encoderbtn_callback(uint16_t value)
{
	btn_val = value;
}
#endif // #ifdef USE_CALLBACK

void sample_process_1_encoder(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint8_t	logo_cnt=0;

	Encoder_Drv.timer = &htim1;
	encoder_driver_handle = encoder_register(&Encoder_Drv);
	button_driver_handle = gpio_int_register(&Button_Driver);
	uart_driver_handle = uart_register(&Uart_Drv);
	uart_start_receive(uart_driver_handle);
#ifdef USE_CALLBACK
	Encoder_Drv.wakeup_id = 0;
#endif // #ifdef USE_CALLBACK

	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);

	while(1)
	{
		wait_event(EVENT_TIMER | EVENT_TIM_IRQ | EVENT_EXT_INT_IRQ);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & EVENT_TIMER) == EVENT_TIMER)
		{
			process_led();
			logo_cnt++;
			if ( logo_cnt == 5 )
			{
				sprintf((char *)uart_tx_buffer,"Encoder test\n\r");
				uart_send(uart_driver_handle, uart_tx_buffer,strlen((char * )uart_tx_buffer));
			}
			if ( logo_cnt > 6 )
				logo_cnt = 6;
#ifdef USE_CALLBACK
			if (( Encoder_Drv.status & ENCODER_READY) == ENCODER_READY)
			{
				Encoder_Drv.status &= ~ENCODER_READY;
				if (( Encoder_Drv.status & ENCODER_UP) == ENCODER_UP)
					sprintf((char *)uart_tx_buffer,"Callback Encoder : %d UP\n\r",(int )Encoder_Drv.encoder_value);
				else
					sprintf((char *)uart_tx_buffer,"Callback Encoder : %d DOWN\n\r",(int )Encoder_Drv.encoder_value);
				uart_send(uart_driver_handle, uart_tx_buffer,strlen((char * )uart_tx_buffer));
			}
			if (( Button_Driver.status & GPIO_INT_EVENT) == GPIO_INT_EVENT)
			{
				Button_Driver.status &= ~GPIO_INT_EVENT;
				sprintf((char *)uart_tx_buffer,"Callback Button\n\r");
				uart_send(uart_driver_handle, uart_tx_buffer,strlen((char * )uart_tx_buffer));
			}

#endif // #ifdef USE_CALLBACK
		}

#ifndef USE_CALLBACK
		if (( wakeup & WAKEUP_FROM_TIM_IRQ) == WAKEUP_FROM_TIM_IRQ)
		{
			if (( Encoder_Drv.status & ENCODER_UP) == ENCODER_UP)
				sprintf((char *)uart_tx_buffer,"Event Encoder : %d UP\n\r",(int )Encoder_Drv.encoder_value);
			else
				sprintf((char *)uart_tx_buffer,"Event Encoder : %d DOWN\n\r",(int )Encoder_Drv.encoder_value);
			uart_send(uart_driver_handle, uart_tx_buffer,strlen((char * )uart_tx_buffer));
		}
		if (( wakeup & WAKEUP_FROM_EXT_INT_IRQ) == WAKEUP_FROM_EXT_INT_IRQ)
		{
			sprintf((char *)uart_tx_buffer,"Event Encoder Button Pressed\n\r");
			uart_send(uart_driver_handle, uart_tx_buffer,strlen((char * )uart_tx_buffer));
		}
#endif // #ifndef USE_CALLBACK
	}
}

#endif // #ifdef 	SAMPLEPROCESS_1_ENCODER

#endif // #ifdef SAMPLE_PROCESSES_ENABLED

