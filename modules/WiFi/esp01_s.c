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
 *  Created on: Jun 3, 2024
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

esp_cmds_t	esp_connect_cmds[] =
{
		{
			.cmd = "AT\r\n",
			.reply = "\r\nOK\r\n",
			.delay = ESP01S_WAIT_10
		},
		{
			.cmd = "AT+CWMODE=3\r\n",
			.reply = "\r\nOK\r\n",
			.delay = ESP01S_WAIT_10
		},
		{
			.cmd = "AT+CWQAP\r\n",
			.reply = "\r\nOK\r\n",
			.delay = ESP01S_WAIT_10
		},
		{
			.cmd = "AT+RST\r\n",
			.reply = "ready\r",
			.delay = ESP01S_WAIT_60
		},
		{
			.cmd = "AT+CWJAP=",
			.reply = "GOT IP",
			.delay = ESP01S_WAIT_60
		},
		{
			.cmd = "AT+CIFSR\r\n",
			.reply = ":STAIP",
			.delay = ESP01S_WAIT_10
		},
		{
			.cmd = "AT+CIPMUX=1\r\n",
			.reply = "\r\nOK\r\n",
			.delay = ESP01S_WAIT_10
		},
		{
			.cmd = "AT+CIPSERVER=1,80\r\n",
			.reply = "\r\nOK\r\n",
			.delay = ESP01S_WAIT_10
		},
		{
			.cmd = "NotUsedCmd",
			.reply = "NotUsedReply",
			.delay = ESP01S_WAIT_LASTCMD
		},
};

esp_cmds_t	esp_web_cmds[] =
{
		{
			.cmd = "AT+CIPSEND=0,",
			.reply = "\r\nOK\r\n",
			.delay = ESP01S_EXT_PARAM
		},
		{
			.cmd = "webdata",
			.reply = "D OK\r\n",
			.delay = ESP01S_EXT_DATA
		},
		{
			.cmd = "AT+CIPCLOSE=0\r\n",
			.reply = "\r\nOK\r\n",
			.delay = ESP01S_WAIT_01
		},
		{
			.cmd = "NotUsedCmd",
			.reply = "NotUsedReply",
			.delay = ESP01S_WAIT_LASTCMD
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
uint16_t	i,j;
	bzero(esp_wifi.ip,ESP01S_IP_LEN);
	bzero(esp_wifi.apmac,ESP01S_MAC_LEN);
	bzero(esp_wifi.mac,ESP01S_MAC_LEN);
	for(i=0;i<strlen((char *)esp_wifi.rx_buf)-7;i++)
	{
		if (strncmp( (char *)&esp_wifi.rx_buf[i],"STAIP,\"",7) == 0)
		{
			for(j=0;j<32;j++)
			{
				if (esp_wifi.rx_buf[i+j+7] == '\"' )
					j=32;
				else
					esp_wifi.ip[j] = esp_wifi.rx_buf[i+j+7];
			}
		}
		if (strncmp( (char *)&esp_wifi.rx_buf[i],"APMAC,\"",7) == 0)
		{
			for(j=0;j<32;j++)
			{
				if (esp_wifi.rx_buf[i+j+7] == '\"' )
					j=32;
				else
					esp_wifi.apmac[j] = esp_wifi.rx_buf[i+j+7];
			}
		}
		if (strncmp( (char *)&esp_wifi.rx_buf[i],"TAMAC,\"",7) == 0)
		{
			for(j=0;j<32;j++)
			{
				if (esp_wifi.rx_buf[i+j+7] == '\"' )
					j=32;
				else
					esp_wifi.mac[j] = esp_wifi.rx_buf[i+j+7];
			}
		}
	}
}


uint8_t ESP01S_Worker(esp_cmds_t *esp_work_in)
{
esp_cmds_t		*esp_work = esp_work_in;
	while(esp_work[esp_wifi.esp01s_worker_state].delay != ESP01S_WAIT_LASTCMD )
	{
		if ((esp_wifi.status & ESP01S_WIFI_CMD_SENT) != ESP01S_WIFI_CMD_SENT)
		{
			bzero(esp_wifi.rx_buf,esp_wifi.rx_buf_max_len);
			if ( esp_work[esp_wifi.esp01s_worker_state].delay == ESP01S_EXT_PARAM )
			{
				bzero(esp_wifi.wparams,ESP01S_WEB_PARAMS_LEN);
				sprintf(esp_wifi.wparams,"%s%d\r\n",esp_work[esp_wifi.esp01s_worker_state].cmd,strlen(esp_wifi.webpage)+2);
				hw_send_uart_dma(esp_wifi.serial_port,(uint8_t *)esp_wifi.wparams,strlen(esp_wifi.wparams));
				esp_wifi.delay = ESP01S_WAIT_01;
			}
			else if ( esp_work[esp_wifi.esp01s_worker_state].delay == ESP01S_EXT_DATA )
			{
				hw_send_uart_dma(esp_wifi.serial_port,(uint8_t *)esp_wifi.webpage,strlen(esp_wifi.webpage));
				esp_wifi.delay = ESP01S_WAIT_01;
			}
			else
			{
				hw_send_uart_dma(esp_wifi.serial_port,(uint8_t *)esp_work[esp_wifi.esp01s_worker_state].cmd,strlen(esp_work[esp_wifi.esp01s_worker_state].cmd));
				esp_wifi.delay = esp_work[esp_wifi.esp01s_worker_state].delay;
			}

			esp_wifi.status |= ESP01S_WIFI_CMD_SENT;
			return ESP01S_WIFI_CMD_OK;
		}
		else if ( ESP01S_AT_reply(esp_work[esp_wifi.esp01s_worker_state].reply) == 0)
		{
			if (strcmp(esp_work[esp_wifi.esp01s_worker_state].reply,":STAIP") == 0 )
			{
				ESP01S_get_mac_ip();
			}
			esp_wifi.status &= ~ESP01S_WIFI_CMD_SENT;
			if (esp_work[esp_wifi.esp01s_worker_state].delay == ESP01S_WAIT_LASTCMD )
			{
				esp_wifi.status |= ESP01S_WIFI_CMDLIST_FINISHED;
				return ESP01S_WIFI_CMD_OK;
			}
			else
			{
				esp_wifi.esp01s_worker_state++;
				esp_wifi.cmd_retry = ESP01S_NUM_RETRY;
			}
			return ESP01S_WIFI_CMD_OK;
		}
		else
		{
			esp_wifi.delay--;
			if ( esp_wifi.delay > ESP01S_WAIT_60)
				esp_wifi.delay = ESP01S_WAIT_60;

			if ( esp_wifi.delay == 0 )
			{
				esp_wifi.cmd_retry--;
				if ( esp_wifi.cmd_retry == 0 )
				{
					esp_wifi.esp01s_worker_state = 0;
					esp_wifi.status &= ~ESP01S_WIFI_CMDLIST_FINISHED;
					esp_wifi.cmd_retry = ESP01S_NUM_RETRY;
					return ESP01S_WIFI_CMD_ERROR;
				}
				else
				{
					esp_wifi.status &= ~ESP01S_WIFI_CMD_SENT;
					esp_wifi.delay = esp_work[esp_wifi.esp01s_worker_state].delay;
				}
			}
			else
				return ESP01S_WIFI_CMD_OK;
		}
	}
	if (esp_work[esp_wifi.esp01s_worker_state].delay == ESP01S_WAIT_LASTCMD )
		esp_wifi.status |= ESP01S_WIFI_CMDLIST_FINISHED;
	return ESP01S_WIFI_CMD_OK;
}

uint8_t ESP01S_Reset_Worker (void)
{
	esp_wifi.status &= ~ESP01S_WIFI_CMDLIST_FINISHED;
	esp_wifi.esp01s_worker_state = 0;
	esp_wifi.delay = 0;
	return esp_wifi.esp01s_worker_state;
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

uint8_t ESP01S_set_webpage (char *webpage)
{
	esp_wifi.webpage = webpage;
	return 0;
}

uint8_t ESP01S_Init (uint8_t serial_port,uint8_t *rx_buf, uint16_t rx_buf_max_len,char *ssid,char *pwd,char *webpage)
{
uint32_t	i=0;
	esp_wifi.rx_buf = rx_buf;
	esp_wifi.webpage = webpage;
	esp_wifi.rx_buf_max_len = rx_buf_max_len;
	esp_wifi.cmd_retry = ESP01S_NUM_RETRY;
	esp_wifi.esp01s_worker_state = 0;
	esp_wifi.esp01s_web_state = 0;
	strcpy((char *)esp_wifi.SSID,ssid);
	strcpy((char *)esp_wifi.PASSWD,pwd);
	sprintf (esp_wifi.data, "\"%s\",\"%s\"\r\n", esp_wifi.SSID, esp_wifi.PASSWD);
	while(esp_connect_cmds[i].delay != ESP01S_WIFI_CMDLIST_FINISHED)
	{
		if ( strcmp(esp_connect_cmds[i].cmd,"AT+CWJAP=") == 0)
		{
			strcat (esp_connect_cmds[i].cmd,esp_wifi.data);
			break;
		}
		i++;
	}
#ifdef WIFI_RESET_GPIOPORT
	HAL_GPIO_WritePin(WIFI_RESET_GPIOPORT, WIFI_RESET_GPIOBIT,GPIO_PIN_RESET);
	task_delay(100);
	HAL_GPIO_WritePin(WIFI_RESET_GPIOPORT, WIFI_RESET_GPIOBIT,GPIO_PIN_SET);
	task_delay(500);
#endif
	esp_wifi.serial_port = serial_port;
	hw_receive_uart(esp_wifi.serial_port,esp_wifi.rx_buf,esp_wifi.rx_buf_max_len,ESP01S_UART_TIMEOUT);
	return 0;

}

#endif // #ifdef WIFI_ESP01S
