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
 * sample_process_1_intflash.c
 *
 *  Created on: Dec 16, 2024
 *      Author: fil
 */
#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_INTFLASH
#ifdef	STM32H743xx

#define	DATA_FLASH_SIZE	1024
#define	FLASH_ADDRESS	(uint8_t *)0x08100000
uint8_t data_for_flash[DATA_FLASH_SIZE];

#define	USB_RX_BUF_SIZE	64
#define	USB_TX_BUF_SIZE	64
uint8_t	usb_rx_buffer[USB_RX_BUF_SIZE];
uint8_t	packed_usb_rx_buffer[USB_RX_BUF_SIZE];
uint8_t	usb_tx_buffer[USB_TX_BUF_SIZE];

USB_Drv_TypeDef	USB_Drv =
{
		.data = usb_rx_buffer,
		.data_index = 0,
		.requested_len = XMODEM_LINE_LEN,
		.timeout = 250,
		.wakeup_id = WAKEUP_FROM_USB_DEVICE_IRQ,
};
uint32_t	usb_driver_handle;
uint8_t		usb_buf_len;

uint8_t		xmodem_usb_enable_poll;
uint8_t		xmodem_usb_reply;
uint8_t		nak=X_NAK,ack=X_ACK;

#define	xmodem_data_area	0x30001000
#define	xmodem_data_len		0x2ffff
uint8_t		xmodem_timeout = XMODEM_TIMEOUT;

#define	COMM_NORMAL_MODE	0
#define	COMM_XMODEM_MODE	1
uint8_t		comm_state = COMM_NORMAL_MODE, wflash = 0;
uint8_t		parsed_cmd;
uint32_t	ndecoded;

#define	CMDPARSER_RET_PRG	1
#define	CMDPARSER_RET_WAV	2
#define	CMDPARSER_RET_RUN	3
#define	CMDPARSER_RET_HLT	4

uint8_t		usb_flags = 0,usb_rx_buffer_index;

uint8_t Host_pack_USB_packet(uint8_t *usb_rx_buffer,uint8_t len)
{
uint32_t	i;

	for(i=0;i<len;i++)
	{
		i &= (USB_RX_BUF_SIZE-1);
		if ( usb_flags == 0 )
		{
			if ( usb_rx_buffer[i] == '<')
			{
				packed_usb_rx_buffer[0] = usb_rx_buffer[i];
				usb_rx_buffer_index = 1;
				usb_flags = 1;
			}
		}
		else
		{
			packed_usb_rx_buffer[usb_rx_buffer_index ] = usb_rx_buffer[i];
			if ( packed_usb_rx_buffer[usb_rx_buffer_index] == '>')
			{
				usb_flags = 0;
				usb_rx_buffer_index = 0;
				return	0;
			}
			usb_rx_buffer_index++;
		}
	}
	return 1;
}

uint8_t command_from_host[8];
uint8_t parse_packet(uint8_t *rx_buf)
{
uint16_t	pnum;

	pnum = sscanf((char * )rx_buf,"< %s >", (char *)command_from_host);
	if ( pnum == 1)
	{
		if ( strcmp((char *)command_from_host,"BIN") == 0 )
			return CMDPARSER_RET_PRG;
	}
	return 0;
}

void sample_process_1_intflash(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint8_t 	*flash_address = FLASH_ADDRESS;
uint32_t 	flash_size;

	usb_driver_handle = usb_device_driver_register(&USB_Drv);
	xmodem_init((uint8_t *)xmodem_data_area,xmodem_data_len);

	create_timer(TIMER_ID_0,500,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);

	while(1)
	{
		wait_event(EVENT_TIMER|EVENT_USB_DEVICE_IRQ);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			if ( comm_state == COMM_XMODEM_MODE)
			{
				if ( xmodem_usb_enable_poll == 1 )
				{
					xmodem_set_data_area((uint8_t *)xmodem_data_area,xmodem_data_len );
					usb_send(usb_driver_handle,&nak,1);
				}
				xmodem_timeout--;
				if ( xmodem_timeout == 0 )
				{
					xmodem_usb_enable_poll = 1;
				}
			}
			if ( comm_state == COMM_NORMAL_MODE)
			{
				if ( wflash )
				{
					wflash = 0;
					flash_address = FLASH_ADDRESS;
					flash_size = (uint32_t )xmodem_get_rxed_amount();
#ifdef FLASH_INTERNAL
					if ( flash_size < (512*1024))
						flash_update((uint8_t *)xmodem_data_area,flash_address,flash_size);
					while(1);
#endif
				}
			}
#ifdef LD1_GPIO_Port
			HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
#endif // #ifdef LD1_GPIO_Port
		}
		if (( wakeup & WAKEUP_FROM_USB_DEVICE_IRQ) == WAKEUP_FROM_USB_DEVICE_IRQ)
		{
			if ( comm_state == COMM_XMODEM_MODE)
			{
				xmodem_usb_enable_poll = 0;
				xmodem_timeout = XMODEM_TIMEOUT;
				usb_buf_len = usb_get_rx_len(usb_driver_handle);
				xmodem_usb_reply = xmodem_line_parser(usb_rx_buffer);
				switch(xmodem_usb_reply)
				{
				case	X_EOT:
					usb_send(usb_driver_handle,&ack,1);
					comm_state = COMM_NORMAL_MODE;
					USB_Drv.data_index = 0;
					wflash = 1;
					break;
				case	X_ACK:
					usb_send(usb_driver_handle,&ack,1);
					break;
				default:
					usb_send(usb_driver_handle,&nak,1);
					break;
				}
			}
			if ( comm_state == COMM_NORMAL_MODE )
			{
				usb_buf_len = usb_get_rx_len(usb_driver_handle);

				if ( Host_pack_USB_packet(usb_rx_buffer,usb_buf_len) == 0 )
				{
					parsed_cmd = parse_packet(packed_usb_rx_buffer);
					switch(parsed_cmd)
					{
					case CMDPARSER_RET_PRG:
						comm_state = COMM_XMODEM_MODE;
						xmodem_usb_enable_poll = 1;
						break;
					}
				}
			}
		}

	}
}

#endif // #ifdef	STM32H743xx
#endif // #ifdef SAMPLEPROCESS_1_INTFLASH
#endif // #ifdef SAMPLE_PROCESSES_ENABLED
