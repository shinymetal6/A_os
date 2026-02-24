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
 * sample_process_1_pn5180.c
 *
 *  Created on: Jan 16, 2026
 *      Author: fil
 */

/*
 * Pin allocation for NUCLEO-G491RE
 * SPI2_MOSI	PB15	CN10-26
 * SPI2_MISO	PB14	CN10-28
 * SPI2_SCK		PB13	CN10-30
 * PN5180_SS	PB12	CN10-16
 * PN5180_BUSY	PB11	CN10-18
 * PN5180_IRQ	PB1		CN10-24
 * PN5180_RESET	PB2		CN10-22
 * NEOLED		PC6		CN10-4
 */

#include "main.h"
#include "sample_A_os_includes.h"

#ifdef SAMPLE_PROCESSES_ENABLED

#include "sample_processes_includes.h"

#ifdef SAMPLEPROCESS_1_PN5180
extern	SPI_HandleTypeDef hspi2;
extern	TIM_HandleTypeDef htim3;

uint8_t txbuf[PN5180_PAYLOAD_LENGTH];
uint8_t rxbuf[PN5180_PAYLOAD_LENGTH];
uint32_t	nfc_irq=0;
uint32_t nfc_irq_callback(uint16_t GPIO_Pin,uint32_t *param)
{
	nfc_irq++;
	return 0;
}

GPIO_Interrupt_DriverStruct_t	pn5180_Int_Driver;
SPI_NFC_DriverStruct_t	SPI_NFC_Driver =
{
		.wakeup_id = 1,
		//.flags = SPI_USES_DMA,
		.bus = &hspi2,
		.spi_timeout_ms = 100,
		.nfc_model = NFC_IS_PN5180,
		.iso_card = NFC_CARD_IS_14443,
		.tx_data_ptr = txbuf,
		.rx_data_ptr = rxbuf,
		.cs_bit = PN5180_SS_Pin,
		.cs_port = PN5180_SS_GPIO_Port,
		.reset_bit = PN5180_RESET_Pin,
		.reset_port = PN5180_RESET_GPIO_Port,
		.busy_bit = PN5180_BUSY_Pin,
		.busy_port = PN5180_BUSY_GPIO_Port,
		.irq_bit = PN5180_IRQ_Pin,
		.irq_port = PN5180_IRQ_GPIO_Port,
		.IRQ_number = EXTI1_IRQn,
		.nfc_irq_driver = &pn5180_Int_Driver,
		.nfc_irq_callback = nfc_irq_callback,
};

#ifdef	STM32G491xx
extern	UART_HandleTypeDef	hlpuart1;
#define	UART				hlpuart1
#define	UART_WAKEUP			WAKEUP_FROM_UART1_IRQ
#define	UART_EVENT			EVENT_UART1_IRQ
#endif

#define	UART_RX_BUF_SIZE	512
#define	UART_TX_BUF_SIZE	512
uint8_t	uart_rx_buffer[UART_RX_BUF_SIZE];
uint8_t	uart_tx_buffer[UART_TX_BUF_SIZE];

UART_Drv_TypeDef Uart_Drv =
{
	.data = uart_rx_buffer,
	.rx_max_len = XMODEM_LINE_LEN,
	.uart = &hlpuart1,
	.wakeup_id = UART_WAKEUP,
	.timeout = 100,
	.flags = UART_USES_DMA_TX | UART_USES_DMA_RX | UART_WAKEUP_ON_RXFULL | UART_WAKEUP_ON_TIMEOUT,
	//.flags = UART_WAKEUP_ON_RXFULL | UART_WAKEUP_ON_TIMEOUT,
};

void sample_process_1_init(uint32_t process_id)
{

}

uint8_t	check_val;
uint8_t Key[6] = {0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF};
uint8_t RdBuf[32];
uint8_t WrBuf[32];
uint32_t init_result;
uint8_t	ss_string[32];
#define	ISO_CYCLE_TIME	10
#define	BLOCK_NUMBER	6

uint8_t	card_sm=0;

void sample_process_1_pn5180(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint8_t		card_time_off;
	create_timer(TIMER_ID_0,200,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	spi_nfc_register(&SPI_NFC_Driver);
	uart_register(&Uart_Drv);
	uart_start_receive(&Uart_Drv);
	uart_send(&Uart_Drv, (uint8_t *)"Initialized\r\n",strlen("Initialized\r\n"));
	ISO14443_CardOn(&SPI_NFC_Driver);

	check_val = rxbuf[0x1a];
	while(1)
	{
		wait_event(EVENT_TIMER | UART_EVENT);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{

			switch(card_sm)
			{
			case 0:
				ISO14443_CardOn(&SPI_NFC_Driver);
				card_sm++;
				break;
			case 1:
				card_sm++;
				break;
			case 2:
			    if ( ISO14443_Discovery(&SPI_NFC_Driver) == 0 )
			    {
			    	sprintf((char *)ss_string,"Discovered : sak = 0x%02x\r\n",SPI_NFC_Driver.SAK);
			    	card_sm++;
			    }
			    else
			    {
			    	card_sm = 7;
			    }
				break;
			case 3:
		    	if ( ISO14443_Authenticate(&SPI_NFC_Driver,Key,NFC_CMD_AUTHA_ISO14443,0) != 0xff)
		    	{
			    	uart_send(&Uart_Drv, (uint8_t *)"Authenticated\r\n",strlen("Authenticated\r\n"));
			    	card_sm++;
		    	}
			    else
			    {
			    	card_sm = 7;
			    }
				break;
			case 4:
	    		bzero(RdBuf,sizeof(RdBuf));
	    		if ( ISO14443_BlockRead(&SPI_NFC_Driver,BLOCK_NUMBER,RdBuf) == 16 )
	    		{
					HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
					if ( RdBuf[0] != 'C')
						card_sm++;
	    		}
	    		else
			    	card_sm = 7;
		    	break;
			case 5:
	    		WrBuf[0] = 'C';
	    		WrBuf[1] = 'i';
	    		WrBuf[2] = 'a';
	    		WrBuf[3] = 'o';
	    		if ( ISO14443_BlockWrite(&SPI_NFC_Driver,BLOCK_NUMBER,WrBuf) )
	    			card_sm = 4;
	    		else
	    			card_sm++;
				break;
			case 6:
				card_sm++;
				break;
			case 7:
				ISO14443_CardOff(&SPI_NFC_Driver);
				card_sm++;
				card_time_off = 10;
				break;
			case 8:
				card_time_off--;
				if ( card_time_off == 0 )
				{
					card_time_off = 10;
					card_sm = 0;
				}
				break;
			default :
				card_sm = 0;
				break;
			}
		}
		if (( wakeup & UART_WAKEUP) == UART_WAKEUP)
		{

		}
	}
}

#endif // #ifdef SAMPLEPROCESS_1_PN5180
#endif // #ifdef SAMPLE_PROCESSES_ENABLED

