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
 * esp01_s.h
 *
 *  Created on: Jun 3, 2024
 *      Author: fil
 */

#ifndef MODULES_WIFI_ESP01_S_H_
#define MODULES_WIFI_ESP01_S_H_

#define	ESP01S_SSID_LEN			32
#define	ESP01S_PWD_LEN			32
#define	ESP01S_REPLY_LEN		80
#define	ESP01S_IP_LEN			16
#define	ESP01S_MAC_LEN			18
#define	ESP01S_CMD_LEN			64
#define	ESP01S_WEB_LEN			1024
#define	ESP01S_WEB_PARAMS_LEN	128

#define	ESP01S_MAX_CONNECT_STATE	10
#define	ESP01S_MAX_WEBREPLY_STATE	10

typedef struct
{
	uint8_t 	status;
	uint8_t 	*rx_buf;
	uint16_t 	rx_buf_max_len;
	uint8_t 	serial_port;
	uint8_t 	esp01s_worker_state;
	uint8_t 	esp01s_web_state;
	uint16_t	delay;
	uint8_t		cmd_retry;
	char		SSID[ESP01S_SSID_LEN];
	char		PASSWD[ESP01S_PWD_LEN];
	char 		data[ESP01S_REPLY_LEN];
	char 		ip[ESP01S_IP_LEN];
	char 		apmac[ESP01S_MAC_LEN];
	char 		mac[ESP01S_MAC_LEN];
	char 		*webpage;
	char		wparams[ESP01S_WEB_PARAMS_LEN];
	char		cmd_webpage[ESP01S_WEB_LEN];
}esp_wifi_t;

/*  status */
#define	ESP01S_WIFI_DISCONNECTED		0x00
#define	ESP01S_WIFI_CMD_SENT			0x01
#define	ESP01S_WEB_ERROR				0x08
#define	ESP01S_WEB_PAGE_SENT			0x10
#define	ESP01S_WEB_CLOSE_PAGE_SENT		0x20
#define	ESP01S_WIFI_CMDLIST_FINISHED	0x40
#define	ESP01S_WIFI_WEB_REQUEST			0x80

#define	ESP01S_WIFI_CMD_OK				0
#define	ESP01S_WIFI_CMD_ERROR			1

#define	ESP01S_WAIT_LASTCMD	0
#define	ESP01S_WAIT_01		1
#define	ESP01S_WAIT_02		2
#define	ESP01S_WAIT_05		5
#define	ESP01S_WAIT_10		10
#define	ESP01S_WAIT_20		20
#define	ESP01S_WAIT_30		30
#define	ESP01S_WAIT_40		40
#define	ESP01S_WAIT_50		50
#define	ESP01S_WAIT_60		60
#define	ESP01S_WAIT_100		100
#define	ESP01S_WAIT_200		200
#define	ESP01S_WAIT_500		500
#define	ESP01S_EXT_PARAM	2000
#define	ESP01S_EXT_DATA		2001

typedef struct
{
	char		cmd[ESP01S_CMD_LEN];
	char 		reply[ESP01S_REPLY_LEN];
	uint16_t	delay;
}esp_cmds_t;

#define	ESP01S_UART_TIMEOUT			20
#define	ESP01S_NUM_RETRY			20

extern	uint8_t ESP01S_Init (uint8_t serial_port,uint8_t *rx_buf, uint16_t rx_buf_max_len,char *ssid,char *pwd,char *webpage);
extern	uint8_t ESP01S_SendCmd(char *cmd);
extern	uint8_t ESP01S_Worker(esp_cmds_t *esp_work);
extern	uint8_t ESP01S_Reset_Worker (void);
extern	uint8_t ESP01S_set_webpage (char *webpage);
extern	uint8_t ESP01S_WebProcess(void);
extern	uint8_t ESP01S_get_state (void);
extern	uint8_t ESP01S_get_IP (uint8_t *ip);
extern	uint8_t ESP01S_get_MAC (uint8_t *mac);

#endif /* MODULES_WIFI_ESP01_S_H_ */
