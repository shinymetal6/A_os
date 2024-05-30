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
 * esp01_s.c
 *
 *  Created on: May 30, 2024
 *      Author: fil
 */


#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"

#ifdef WIFI_ESP01S

#include "esp01_s.h"
#include <string.h>

esp_wifi_t	esp_wifi;
#define	ESP01S_MAX_STATE	10
esp_cmds_t	esp_cmds[ESP01S_MAX_STATE] =
{
		{
			.cmd = "AT\r\n",
			.reply = "\r\nOK\r\n",
			.delay = ESP01S_WAIT_10
		},
		{
			.cmd = "AT\r\n",
			.reply = "\r\nOK\r\n",
			.delay = ESP01S_WAIT_10
		},
		{
			.cmd = "AT+CWMODE=3\r\n",
			.reply = "\r\nOK\r\n",
			.delay = ESP01S_WAIT_100
		},
		{
			.cmd = "AT+CWQAP\r\n",
			.reply = "\r\nOK\r\n",
			.delay = ESP01S_WAIT_100
		},
		{
			.cmd = "AT+RST\r\n",
			.reply = "ready\r",
			.delay = ESP01S_WAIT_500
		},
		{
			.cmd = "AT+CWJAP=",
			.reply = "GOT IP",
			.delay = ESP01S_WAIT_500
		},
		{
			.cmd = "AT+CIFSR\r\n",
			.reply = ":STAIP",
			.delay = ESP01S_WAIT_05
		},
		{
			.cmd = "AT+CIPMUX=1\r\n",
			.reply = "\r\nOK\r\n",
			.delay = ESP01S_WAIT_05
		},
		{
			.cmd = "AT+CIPSERVER=1,80\r\n",
			.reply = "\r\nOK\r\n",
			.delay = ESP01S_WAIT_05
		},
		{
			.cmd = "NotUsedCmd",
			.reply = "NotUsedReply",
			.delay = 0
		},
};


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

void ESP01S_get_mac_ip(void)
{
uint16_t	i;
	bzero(esp_wifi.ip,ESP01S_IP_LEN);
	for(i=25;i<41;i++)
	{
		if ((( esp_wifi.rx_buf[i] >= 0x30 ) && ( esp_wifi.rx_buf[i] <= 0x39 )) | ( esp_wifi.rx_buf[i] == '.' ))
			esp_wifi.ip[i-25] = esp_wifi.rx_buf[i];
		else
			break;
	}
	bzero(esp_wifi.mac,ESP01S_MAC_LEN);
	for(i=57;i<74;i++)
	{
		esp_wifi.mac[i-57] = esp_wifi.rx_buf[i];
	}

}
uint8_t ESP01S_Process(uint8_t	force_state)
{
	if ( force_state < ESP01S_MAX_STATE )
	{
		if ((esp_wifi.status & ESP01S_WIFI_CMD_SENT) != ESP01S_WIFI_CMD_SENT)
		{
			if ( esp_cmds[force_state].delay == 0 )
			{
				esp_wifi.status |= ESP01S_WIFI_CONNECTED;
				ESP01S_get_mac_ip();
				return force_state;
			}
			else
			{
				esp_wifi.delay = esp_cmds[force_state].delay;
				bzero(esp_wifi.rx_buf,esp_wifi.rx_buf_max_len);
				hw_send_uart_dma(esp_wifi.serial_port,(uint8_t *)esp_cmds[force_state].cmd,strlen(esp_cmds[force_state].cmd));
				esp_wifi.status |= ESP01S_WIFI_CMD_SENT;
			}
		}
		if ( ESP01S_AT_reply(esp_cmds[force_state].reply) == 0)
		{
			esp_wifi.status &= ~ESP01S_WIFI_CMD_SENT;
			return force_state+1;
		}
		else
		{
			esp_wifi.delay--;
			if ( esp_wifi.delay == 0 )
			{
				esp_wifi.cmd_retry --;
				if ( esp_wifi.cmd_retry == 0 )
				{
					force_state = 0;
					esp_wifi.status = 0;
					esp_wifi.delay = esp_cmds[force_state].delay;
				}
				else
				{
					esp_wifi.status &= ~ESP01S_WIFI_CMD_SENT;
					esp_wifi.delay = esp_cmds[force_state].delay;
				}
			}
		}
	}
	return force_state;
}


uint8_t ESP01S_get_state (void)
{
	return esp_wifi.status;
}

uint8_t ESP01S_get_IP (uint8_t *ip)
{
	strcpy((char *)ip, esp_wifi.ip);
	return 0;
}

uint8_t ESP01S_get_MAC (uint8_t *mac)
{
	strcpy((char *)mac, esp_wifi.mac);
	return 0;
}

uint8_t ESP01S_Init (uint8_t serial_port,uint8_t *rx_buf, uint16_t rx_buf_max_len,char *ssid,char *pwd)
{
	esp_wifi.rx_buf = rx_buf;
	esp_wifi.rx_buf_max_len = rx_buf_max_len;
	esp_wifi.cmd_retry = 5;
	strcpy((char *)esp_wifi.SSID,ssid);
	strcpy((char *)esp_wifi.PASSWD,pwd);
	sprintf (esp_wifi.data, "\"%s\",\"%s\"\r\n", esp_wifi.SSID, esp_wifi.PASSWD);
	strcat (esp_cmds[5].cmd,esp_wifi.data);

/**/
	HAL_GPIO_WritePin(WIFI_RESET_GPIOPORT, WIFI_RESET_GPIOBIT,GPIO_PIN_RESET);
	task_delay(100);
	/**/
	HAL_GPIO_WritePin(WIFI_RESET_GPIOPORT, WIFI_RESET_GPIOBIT,GPIO_PIN_SET);
	task_delay(500);

	esp_wifi.serial_port = serial_port;
	hw_receive_uart(esp_wifi.serial_port,esp_wifi.rx_buf,esp_wifi.rx_buf_max_len,ESP01S_UART_TIMEOUT);
	return 0;

}
#endif // #ifdef WIFI_ESP01S
