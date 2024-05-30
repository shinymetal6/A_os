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
 * esp01s.h
 *
 *  Created on: May 28, 2024
 *      Author: fil
 */

#ifndef MODULES_WIFI_ESP01S_H_
#define MODULES_WIFI_ESP01S_H_


#define	SSID_LEN	32
#define	PWD_LEN		32
#define	REPLY_LEN	80
#define	IP_LEN		16
#define	MAC_LEN		18
typedef struct
{
	uint8_t 	status;
	uint8_t 	*rx_buf;
	uint16_t 	rx_buf_max_len;
	uint8_t 	serial_port;
	uint8_t		esp01s_sm;
	uint16_t	counter;
	char		SSID[SSID_LEN];
	char		PASSWD[PWD_LEN];
	char 		data[REPLY_LEN];
	char 		ip[IP_LEN];
	char 		mac[MAC_LEN];
}esp_wifi_t;

/*  status */
#define	ESP01S_WIFI_DISCONNECTED	0x00
#define	ESP01S_WIFI_CONNECTED		0x01


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

#define	ESP01S_UART_TIMEOUT			20

extern	uint8_t ESP01S_Init (uint8_t serial_port,uint8_t *rx_buf, uint16_t rx_buf_max_len,char *ssid,char *pwd);
extern	uint8_t ESP01S_SendCmd(char *cmd);
extern	uint8_t ESP01S_Process(uint8_t	force_state);
extern	uint8_t ESP01S_get_state (void);
extern	uint8_t ESP01S_get_IP (uint8_t *ip);
extern	uint8_t ESP01S_get_MAC (uint8_t *mac);



#endif /* MODULES_WIFI_ESP01S_H_ */
