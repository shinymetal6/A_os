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
 *  Created on: May 30, 2024
 *      Author: fil
 */

#ifndef MODULES_WIFI_ESP01_S_H_
#define MODULES_WIFI_ESP01_S_H_

#define	ESP01S_SSID_LEN	32
#define	ESP01S_PWD_LEN		32
#define	ESP01S_REPLY_LEN	80
#define	ESP01S_IP_LEN		16
#define	ESP01S_MAC_LEN		18
#define	ESP01S_CMD_LEN		64
typedef struct
{
	uint8_t 	status;
	uint8_t 	*rx_buf;
	uint16_t 	rx_buf_max_len;
	uint8_t 	serial_port;
	uint16_t	delay;
	uint8_t		cmd_retry;
	char		SSID[ESP01S_SSID_LEN];
	char		PASSWD[ESP01S_PWD_LEN];
	char 		data[ESP01S_REPLY_LEN];
	char 		ip[ESP01S_IP_LEN];
	char 		mac[ESP01S_MAC_LEN];
}esp_wifi_t;

/*  status */
#define	ESP01S_WIFI_DISCONNECTED	0x00
#define	ESP01S_WIFI_CMD_SENT		0x01
#define	ESP01S_WIFI_CONNECTED		0x40
#define	ESP01S_WIFI_WEB_REQUEST		0x80

typedef struct
{
	char		cmd[ESP01S_CMD_LEN];
	char 		reply[ESP01S_REPLY_LEN];
	uint16_t	delay;
}esp_cmds_t;

#define	ESP01S_SM_INIT				0
#define	ESP01S_SM_SWRESET			1
#define	ESP01S_SM_SENT_SWRESET		2
#define	ESP01S_SM_SENT_CWLAP		3
#define	ESP01S_SM_SENT_CWMODE		4
#define	ESP01S_SM_SENT_USR_PASSWD	5
#define	ESP01S_SM_WAIT_GOTIP		6
#define	ESP01S_SM_SENT_CIFSR		7
#define	ESP01S_SM_WAIT_CIPMUX		8
#define	ESP01S_SM_WAIT_CIPSERVER	9
#define	ESP01S_SM_CONNECTED			10
#define	ESP01S_SM_IDLE_CONNECTED	11

#define	ESP01S_UART_TIMEOUT			50

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
#define	ESP01S_WAIT_2000	2000
#define	ESP01S_WAIT_5000	5000

extern	uint8_t ESP01S_Init (uint8_t serial_port,uint8_t *rx_buf, uint16_t rx_buf_max_len,char *ssid,char *pwd);
extern	uint8_t ESP01S_SendCmd(char *cmd);
extern	uint8_t ESP01S_Process(uint8_t	force_state);
extern	uint8_t ESP01S_get_state (void);
extern	uint8_t ESP01S_get_IP (uint8_t *ip);
extern	uint8_t ESP01S_get_MAC (uint8_t *mac);


#endif /* MODULES_WIFI_ESP01_S_H_ */
