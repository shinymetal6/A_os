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
 * esp01s.c
 *
 *  Created on: May 28, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"

#ifdef WIFI_ESP01S

#include "esp01s.h"
#include <string.h>
extern void task_delay(uint32_t tick_count);

esp_wifi_t	esp_wifi;

uint8_t ESP01S_SendCmd(char *cmd)
{
	//bzero(esp_wifi.rx_buf,esp_wifi.rx_buf_max_len);
	return hw_send_uart_dma(esp_wifi.serial_port,(uint8_t *)cmd,strlen(cmd));
}

#define	WAIT_05		5
#define	WAIT_10		10
#define	WAIT_20		20
#define	WAIT_30		30
#define	WAIT_40		40
#define	WAIT_50		50
#define	WAIT_60		60
#define	WAIT_100	100
#define	WAIT_200	200
#define	WAIT_500	500

uint8_t ESP01S_WaitReply(char *reply,uint16_t len)
{
	return strncmp(reply,(char *)esp_wifi.rx_buf,len);
}

uint8_t ESP01S_AT_reply(char *reply)
{
uint16_t	i;

	for(i=0;i<esp_wifi.rx_buf_max_len-6;i++)
	{
		if (( esp_wifi.rx_buf[i]==reply[0])&&( esp_wifi.rx_buf[i+1]==reply[1])&&( esp_wifi.rx_buf[i+2]==reply[2])&&( esp_wifi.rx_buf[i+3]==reply[3])&&( esp_wifi.rx_buf[i+4]==reply[4])&&( esp_wifi.rx_buf[i+5]==reply[5]))
			return 0;
	}
	return 1;
}

void ESP01S_sm_check(void)
{
	esp_wifi.counter--;
	if (esp_wifi.counter == 0)
	{
		esp_wifi.counter = WAIT_05;
		esp_wifi.esp01s_sm = ESP01S_SM_INIT;
	}
}

uint8_t ESP01S_Process(uint8_t	force_state)
{
uint8_t	i;
	if ( force_state )
		esp_wifi.esp01s_sm = ESP01S_SM_INIT;

	switch(esp_wifi.esp01s_sm)
	{
	case	ESP01S_SM_INIT	:
		esp_wifi.status = ESP01S_WIFI_DISCONNECTED;
		esp_wifi.counter--;
		if (esp_wifi.counter == 0)
		{
			ESP01S_SendCmd("AT+RST\r\n");
			esp_wifi.esp01s_sm = ESP01S_SM_SWRESET;
			esp_wifi.counter = WAIT_100;
		}
		break;
	case	ESP01S_SM_SWRESET	:
		if ( ESP01S_AT_reply("ready\r") == 0 )
		{
			ESP01S_SendCmd("AT\r\n");
			esp_wifi.counter = 0;
			esp_wifi.esp01s_sm = ESP01S_SM_SENT_SWRESET;
			esp_wifi.counter = WAIT_05;
		}
		else
			ESP01S_sm_check();
		break;
	case	ESP01S_SM_SENT_SWRESET	:
		if ( ESP01S_AT_reply("\r\nOK\r\n") == 0 )
		{
			ESP01S_SendCmd("AT+CWLAP\r\n");
			esp_wifi.esp01s_sm = ESP01S_SM_SENT_CWLAP;
			esp_wifi.counter = WAIT_30;
		}
		else
			ESP01S_sm_check();
		break;
	case	ESP01S_SM_SENT_CWLAP	:
		if ( ESP01S_AT_reply("\r\nOK\r\n") == 0 )
		{
			ESP01S_SendCmd("AT+CWMODE=1\r\n");
			esp_wifi.esp01s_sm = ESP01S_SM_SENT_CWMODE;
			esp_wifi.counter = WAIT_30;
		}
		else
			ESP01S_sm_check();
		break;
	case	ESP01S_SM_SENT_CWMODE	:
		if ( ESP01S_AT_reply("\r\nOK\r\n") == 0 )
		{
			sprintf (esp_wifi.data, "AT+CWJAP=\"%s\",\"%s\"\r\n", esp_wifi.SSID, esp_wifi.PASSWD);
			ESP01S_SendCmd(esp_wifi.data);
			esp_wifi.esp01s_sm = ESP01S_SM_WAIT_GOTIP;
			esp_wifi.counter = WAIT_200;
		}
		else
			ESP01S_sm_check();
		break;
	case	ESP01S_SM_SENT_USR_PASSWD	:
		if ( ESP01S_AT_reply("FI CON") == 0 )
		{
			esp_wifi.esp01s_sm = ESP01S_SM_WAIT_GOTIP;
			esp_wifi.counter = WAIT_500;
		}
		else if ( ESP01S_AT_reply("FI DIS") == 0 )
		{
			ESP01S_SendCmd("AT+RST\r\n");
			esp_wifi.esp01s_sm = ESP01S_SM_SWRESET;
			esp_wifi.counter = WAIT_10;
		}
		else
			ESP01S_sm_check();
		break;
	case	ESP01S_SM_WAIT_GOTIP	:
		if ( ESP01S_AT_reply("GOT IP") == 0 )
		{
			ESP01S_SendCmd("AT+CIFSR\r\n");
			esp_wifi.esp01s_sm = ESP01S_SM_SENT_CIFSR;
			esp_wifi.counter = WAIT_100;
		}
		else
			ESP01S_sm_check();
		break;
	case	ESP01S_SM_SENT_CIFSR	:
		esp_wifi.counter --;
		if ( esp_wifi.counter == 0 )
		{
			if ( ESP01S_AT_reply("busy p") == 0 )
			{
				ESP01S_SendCmd("AT+CIFSR\r\n");
				esp_wifi.esp01s_sm = ESP01S_SM_SENT_CIFSR;
				esp_wifi.counter = WAIT_10;
			}
			else if ( ESP01S_AT_reply(":STAIP") == 0 )
			{
				ESP01S_SendCmd("AT+CIPMUX=1\r\n");
				esp_wifi.esp01s_sm = ESP01S_SM_WAIT_CIPMUX;
				esp_wifi.counter = WAIT_10;
			}
			else
			{
				ESP01S_SendCmd("AT+CIFSR\r\n");
				esp_wifi.esp01s_sm = ESP01S_SM_SENT_CIFSR;
				esp_wifi.counter = WAIT_10;
			}
		}
		break;
	case	ESP01S_SM_WAIT_CIPMUX	:
		if ( ESP01S_AT_reply("\r\nOK\r\n") == 0 )
		{
			ESP01S_SendCmd("AT+CIPSERVER=1,80\r\n");
			esp_wifi.esp01s_sm = ESP01S_SM_WAIT_CIPSERVER;
		}
		break;
	case	ESP01S_SM_WAIT_CIPSERVER	:
		if ( ESP01S_AT_reply("\r\nOK\r\n") == 0 )
		{
			esp_wifi.esp01s_sm = ESP01S_SM_CONNECTED;
		}
		break;
	case	ESP01S_SM_CONNECTED	:
		esp_wifi.status |= ESP01S_WIFI_CONNECTED;
		bzero(esp_wifi.ip,IP_LEN);
		for(i=25;i<41;i++)
		{
			if ((( esp_wifi.rx_buf[i] >= 0x30 ) && ( esp_wifi.rx_buf[i] <= 0x39 )) | ( esp_wifi.rx_buf[i] == '.' ))
				esp_wifi.ip[i-25] = esp_wifi.rx_buf[i];
			else
				break;
		}
		bzero(esp_wifi.mac,MAC_LEN);
		for(i=57;i<74;i++)
		{
			esp_wifi.mac[i-57] = esp_wifi.rx_buf[i];
		}
		break;
	}
	return esp_wifi.esp01s_sm;
}

uint8_t ESP01S_get_state (void)
{
	return esp_wifi.status;
}

uint8_t ESP01S_get_IP (uint8_t *ip)
{
	strcpy(ip, esp_wifi.ip);
	return 0;
}

uint8_t ESP01S_get_MAC (uint8_t *mac)
{
	strcpy(mac, esp_wifi.mac);
	return 0;
}

uint8_t ESP01S_Init (uint8_t serial_port,uint8_t *rx_buf, uint16_t rx_buf_max_len,char *ssid,char *pwd)
{
	esp_wifi.rx_buf = rx_buf;
	esp_wifi.rx_buf_max_len = rx_buf_max_len;
	esp_wifi.esp01s_sm = ESP01S_SM_INIT;
	esp_wifi.counter = WAIT_05;
	strcpy((char *)esp_wifi.SSID,ssid);
	strcpy((char *)esp_wifi.PASSWD,pwd);
/*
	HAL_GPIO_WritePin(WIFI_RESET_GPIOPORT, WIFI_RESET_GPIOBIT,GPIO_PIN_RESET);
	task_delay(100);
	*/
	HAL_GPIO_WritePin(WIFI_RESET_GPIOPORT, WIFI_RESET_GPIOBIT,GPIO_PIN_SET);
	task_delay(500);

	esp_wifi.serial_port = serial_port;
	hw_receive_uart(esp_wifi.serial_port,esp_wifi.rx_buf,esp_wifi.rx_buf_max_len,ESP01S_UART_TIMEOUT);
	return 0;
#ifdef TEMPORARY
	sprintf((char *)tx_buf,"AT+RST\r\n");
	hw_send_uart(bus_peripheral,tx_buf,strlen((char *)tx_buf));
	task_delay(5000);
	/*
	Uart_sendstring("AT+RST\r\n", wifi_uart);
	Uart_sendstring("RESETTING.", pc_uart);
	for (int i=0; i<5; i++)
	{
		Uart_sendstring(".", pc_uart);
		HAL_Delay(1000);
	}
	*/

	/********* AT **********/
	sprintf((char *)tx_buf,"AT\r\n");
	hw_send_uart(bus_peripheral,tx_buf,strlen((char *)tx_buf));
	/*
	Uart_sendstring("AT\r\n", wifi_uart);
	while(!(Wait_for("AT\r\r\n\r\nOK\r\n", wifi_uart)));
	Uart_sendstring("AT---->OK\n\n", pc_uart);
	*/
#ifdef NEXT

	/********* AT+CWMODE=1 **********/
	Uart_sendstring("AT+CWMODE=1\r\n", wifi_uart);
	while (!(Wait_for("AT+CWMODE=1\r\r\n\r\nOK\r\n", wifi_uart)));
	Uart_sendstring("CW MODE---->1\n\n", pc_uart);


	/********* AT+CWJAP="SSID","PASSWD" **********/
	Uart_sendstring("connecting... to the provided AP\n", pc_uart);
	sprintf (data, "AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASSWD);
	Uart_sendstring(data, wifi_uart);
	while (!(Wait_for("WIFI GOT IP\r\n\r\nOK\r\n", wifi_uart)));
	sprintf (data, "Connected to,\"%s\"\n\n", SSID);
	Uart_sendstring(data,pc_uart);


	/********* AT+CIFSR **********/
	Uart_sendstring("AT+CIFSR\r\n", wifi_uart);
	while (!(Wait_for("CIFSR:STAIP,\"", wifi_uart)));
	while (!(Copy_upto("\"",buffer, wifi_uart)));
	while (!(Wait_for("OK\r\n", wifi_uart)));
	int len = strlen (buffer);
	buffer[len-1] = '\0';
	sprintf (data, "IP ADDR: %s\n\n", buffer);
	Uart_sendstring(data, pc_uart);


	Uart_sendstring("AT+CIPMUX=1\r\n", wifi_uart);
	while (!(Wait_for("AT+CIPMUX=1\r\r\n\r\nOK\r\n", wifi_uart)));
	Uart_sendstring("CIPMUX---->OK\n\n", pc_uart);

	Uart_sendstring("AT+CIPSERVER=1,80\r\n", wifi_uart);
	while (!(Wait_for("OK\r\n", wifi_uart)));
	Uart_sendstring("CIPSERVER---->OK\n\n", pc_uart);

	Uart_sendstring("Now Connect to the IP ADRESS\n\n", pc_uart);

#endif // #ifdef NEXT
#endif // #ifdef TEMPORARY
}
#endif // #ifdef WIFI_ESP01S
