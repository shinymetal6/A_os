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
 * mqtt_client.c
 *
 *  Created on: Apr 11, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"

#ifdef MQTT_ENABLE
#include "mqtt_client.h"

#ifdef	STM32H743xx
#include "../../modules/lwip/LwIp/src/include/lwip/apps/mqtt.h"
#endif

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

A_MQTT_SubInfo_t		A_MQTT_SubInfo;
static	mqtt_client_t	*mqtt_client;
struct mqtt_connect_client_info_t mqtt_client_info;

static void mqtt_client_connection_callback(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
}
void mqtt_client_init(uint8_t *broker_ip_addr,char *topic,char *client_identity, char *client_user, char *client_pass)
{
ip_addr_t Subscriber_ip;
	mqtt_client = mqtt_client_new();
	A_MQTT_SubInfo.ip_addrhh = broker_ip_addr[0];
	A_MQTT_SubInfo.ip_addrhl = broker_ip_addr[1];
	A_MQTT_SubInfo.ip_addrlh = broker_ip_addr[2];
	A_MQTT_SubInfo.ip_addrll = broker_ip_addr[3];
	A_MQTT_SubInfo.mqtt_err_c = 0;
	A_MQTT_SubInfo.max_collisions = MAX_COLLISIONS;
	IP4_ADDR(&Subscriber_ip, A_MQTT_SubInfo.ip_addrhh, A_MQTT_SubInfo.ip_addrhl, A_MQTT_SubInfo.ip_addrlh, A_MQTT_SubInfo.ip_addrll);

	memset(&mqtt_client_info, 0, sizeof(mqtt_client_info));

	strcpy(A_MQTT_SubInfo.topic,topic);
	strcpy(A_MQTT_SubInfo.client_identity,client_identity);
	mqtt_client_info.client_id = A_MQTT_SubInfo.client_identity;
	if ( client_user != NULL )
	{
		strcpy(A_MQTT_SubInfo.client_user,client_user);
		mqtt_client_info.client_user = A_MQTT_SubInfo.client_user;
	}
	if ( client_pass != NULL )
	{
		strcpy(A_MQTT_SubInfo.client_pass,client_pass);
		mqtt_client_info.client_pass = A_MQTT_SubInfo.client_pass;
	}
	A_MQTT_SubInfo.mqtt_err_c = mqtt_client_connect(mqtt_client, &Subscriber_ip, MQTT_PORT, mqtt_client_connection_callback, 0, &mqtt_client_info);
}

void mqtt_client_set_flags(uint8_t qos,uint8_t retain,uint8_t max_collisions,uint8_t retry_time_after_collision)
{
	A_MQTT_SubInfo.qos = qos;
	A_MQTT_SubInfo.retain = retain;
	A_MQTT_SubInfo.max_collisions = max_collisions;
	A_MQTT_SubInfo.retry_time_after_collision = retry_time_after_collision;
}

static void mqtt_client_pub_request_callback(void *arg, err_t result)
{
	if(result != ERR_OK)
		A_MQTT_SubInfo.mqtt_err_c++;
}

extern	void task_delay(uint32_t tick_count);

uint32_t mqtt_client_send(char *topic, char *message,uint32_t message_len)
{
ip_addr_t Subscriber_ip;
char	arg[32] = "mqtt_arg";
int8_t	err;

	__disable_irq();
	A_MQTT_SubInfo.collisions = 0;
	while (( Asys.general_flags & LWIP_LOCK) == LWIP_LOCK)
	{
		__enable_irq();
		task_delay(1);
		A_MQTT_SubInfo.collisions++;
		if ( A_MQTT_SubInfo.collisions > A_MQTT_SubInfo.max_collisions )
			return 0;
	}
	__disable_irq();
	if (( Asys.general_flags & LWIP_LOCK) != LWIP_LOCK)
	{
		Asys.general_flags |= LWIP_LOCK;
		__enable_irq();
		if ( mqtt_client_is_connected(mqtt_client) != 1 )
		{
			IP4_ADDR(&Subscriber_ip, A_MQTT_SubInfo.ip_addrhh, A_MQTT_SubInfo.ip_addrhl, A_MQTT_SubInfo.ip_addrlh, A_MQTT_SubInfo.ip_addrll);
			err = mqtt_client_connect(mqtt_client, &Subscriber_ip, MQTT_PORT, mqtt_client_connection_callback, 0, &mqtt_client_info);
			if ( err != 0 )
				A_MQTT_SubInfo.mqtt_err_c++;
		}
		err = mqtt_publish(mqtt_client, topic, message, strlen(message), 0, 0, mqtt_client_pub_request_callback, arg);
		if ( err != 0 )
			message_len = 0;
		Asys.general_flags &= ~LWIP_LOCK;
	}
	else
	{
		message_len = 0;
		__enable_irq();
	}
	return message_len;
}

uint32_t mqtt_client_change_identity(char *client_identity)
{
ip_addr_t Subscriber_ip;
	if ( client_identity == NULL )
		return 1;
	mqtt_disconnect(mqtt_client);
	A_MQTT_SubInfo.mqtt_err_c = 0;
	A_MQTT_SubInfo.max_collisions = MAX_COLLISIONS;
	IP4_ADDR(&Subscriber_ip, A_MQTT_SubInfo.ip_addrhh, A_MQTT_SubInfo.ip_addrhl, A_MQTT_SubInfo.ip_addrlh, A_MQTT_SubInfo.ip_addrll);
	strcpy(A_MQTT_SubInfo.client_identity,client_identity);
	mqtt_client_info.client_id = A_MQTT_SubInfo.client_identity;
	A_MQTT_SubInfo.mqtt_err_c = mqtt_client_connect(mqtt_client, &Subscriber_ip, MQTT_PORT, mqtt_client_connection_callback, 0, &mqtt_client_info);
	return (uint32_t )A_MQTT_SubInfo.mqtt_err_c;
}

uint32_t mqtt_client_change_credentials(char *client_user, char *client_pass)
{
ip_addr_t Subscriber_ip;
	if (( client_user == NULL ) || ( client_pass != NULL ))
		return 1;
	mqtt_disconnect(mqtt_client);
	A_MQTT_SubInfo.mqtt_err_c = 0;
	A_MQTT_SubInfo.max_collisions = MAX_COLLISIONS;
	IP4_ADDR(&Subscriber_ip, A_MQTT_SubInfo.ip_addrhh, A_MQTT_SubInfo.ip_addrhl, A_MQTT_SubInfo.ip_addrlh, A_MQTT_SubInfo.ip_addrll);
	strcpy(A_MQTT_SubInfo.client_user,client_user);
	mqtt_client_info.client_user = A_MQTT_SubInfo.client_user;
	strcpy(A_MQTT_SubInfo.client_pass,client_pass);
	mqtt_client_info.client_pass = A_MQTT_SubInfo.client_pass;
	A_MQTT_SubInfo.mqtt_err_c = mqtt_client_connect(mqtt_client, &Subscriber_ip, MQTT_PORT, mqtt_client_connection_callback, 0, &mqtt_client_info);
	return (uint32_t )A_MQTT_SubInfo.mqtt_err_c;
}

uint32_t mqtt_client_change_broker(char *broker_ip_addr)
{
ip_addr_t Subscriber_ip;

	if ( broker_ip_addr == NULL )
		return 1;
	mqtt_disconnect(mqtt_client);
	A_MQTT_SubInfo.ip_addrhh = broker_ip_addr[0];
	A_MQTT_SubInfo.ip_addrhl = broker_ip_addr[1];
	A_MQTT_SubInfo.ip_addrlh = broker_ip_addr[2];
	A_MQTT_SubInfo.ip_addrll = broker_ip_addr[3];
	A_MQTT_SubInfo.mqtt_err_c = 0;
	A_MQTT_SubInfo.max_collisions = MAX_COLLISIONS;
	IP4_ADDR(&Subscriber_ip, A_MQTT_SubInfo.ip_addrhh, A_MQTT_SubInfo.ip_addrhl, A_MQTT_SubInfo.ip_addrlh, A_MQTT_SubInfo.ip_addrll);
	mqtt_client_info.client_id = A_MQTT_SubInfo.client_identity;
	A_MQTT_SubInfo.mqtt_err_c = mqtt_client_connect(mqtt_client, &Subscriber_ip, MQTT_PORT, mqtt_client_connection_callback, 0, &mqtt_client_info);
	return (uint32_t )A_MQTT_SubInfo.mqtt_err_c;
}

#endif

