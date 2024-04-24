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
 * mqtt_client.h
 *
 *  Created on: Apr 24, 2024
 *      Author: fil
 */

#ifndef MODULES_MQTT_CLIENT_MQTT_CLIENT_H_
#define MODULES_MQTT_CLIENT_MQTT_CLIENT_H_

typedef struct
{
	uint8_t		ip_addrhh;
	uint8_t		ip_addrhl;
	uint8_t		ip_addrlh;
	uint8_t		ip_addrll;
	uint8_t		connected;
	uint8_t		qos;
	uint8_t		retain;
	uint32_t	mqtt_err_c;
	char		client_identity[32];
	char		client_user[32];
	char		client_pass[32];
	char		topic[32];
	uint8_t		max_collisions;
	uint32_t	collisions;
	uint8_t		retry_time_after_collision;
	char		*mqtt_incoming_data_ptr;
}A_MQTT_SubInfo_t;

#define	MAX_COLLISIONS	20
extern	uint8_t mqtt_client_init(uint8_t *broker_ip_addr,char *topic,char *client_identity, char *client_user, char *client_pass, char *mqtt_incoming_data_ptr);
extern	uint32_t mqtt_client_check_connect(void);
extern	uint32_t mqtt_client_send(char *topic, char *message,uint32_t message_len);



#endif /* MODULES_MQTT_CLIENT_MQTT_CLIENT_H_ */
