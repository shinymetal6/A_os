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
#include "../../modules/lwip/STM32H7/LwIp/src/include/lwip/apps/mqtt.h"
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

//void mqtt_client_init(uint8_t ip_addrhh,uint8_t ip_addrhl,uint8_t ip_addrlh,uint8_t ip_addrll,char *topic,char *client_identity)
void mqtt_client_init(uint8_t *broker_ip_addr,char *topic,char *client_identity)
{
ip_addr_t Subscriber_ip;
	mqtt_client = mqtt_client_new();
	A_MQTT_SubInfo.ip_addrhh = broker_ip_addr[0];
	A_MQTT_SubInfo.ip_addrhl = broker_ip_addr[1];
	A_MQTT_SubInfo.ip_addrlh = broker_ip_addr[2];
	A_MQTT_SubInfo.ip_addrll = broker_ip_addr[3];
	IP4_ADDR(&Subscriber_ip, A_MQTT_SubInfo.ip_addrhh, A_MQTT_SubInfo.ip_addrhl, A_MQTT_SubInfo.ip_addrlh, A_MQTT_SubInfo.ip_addrll);

	memset(&mqtt_client_info, 0, sizeof(mqtt_client_info));

	sprintf((char *)mqtt_client_info.client_id,client_identity,strlen(client_identity));

	A_MQTT_SubInfo.mqtt_err_c = mqtt_client_connect(mqtt_client, &Subscriber_ip, MQTT_PORT, mqtt_client_connection_callback, 0, &mqtt_client_info);
}

void mqtt_client_set_flags(uint8_t qos,uint8_t retain)
{
	A_MQTT_SubInfo.qos = qos;
	A_MQTT_SubInfo.retain = retain;
}

static void mqtt_client_pub_request_callback(void *arg, err_t result)
{
	if(result != ERR_OK)
		A_MQTT_SubInfo.mqtt_err_c++;
}

void mqtt_client_send(char *topic, char *message,uint32_t message_len)
{
ip_addr_t Subscriber_ip;
char	arg[32] = "argz";

	if ( mqtt_client_is_connected(mqtt_client) != 1 )
	{
		IP4_ADDR(&Subscriber_ip, A_MQTT_SubInfo.ip_addrhh, A_MQTT_SubInfo.ip_addrhl, A_MQTT_SubInfo.ip_addrlh, A_MQTT_SubInfo.ip_addrll);
		A_MQTT_SubInfo.mqtt_err_c = mqtt_client_connect(mqtt_client, &Subscriber_ip, MQTT_PORT, mqtt_client_connection_callback, 0, &mqtt_client_info);
	}
	A_MQTT_SubInfo.mqtt_err_c = mqtt_publish(mqtt_client, topic, message, strlen(message), 0, 0, mqtt_client_pub_request_callback, arg);
	if ( A_MQTT_SubInfo.mqtt_err_c != 0 )
	{
		mqtt_disconnect(mqtt_client);
		IP4_ADDR(&Subscriber_ip, A_MQTT_SubInfo.ip_addrhh, A_MQTT_SubInfo.ip_addrhl, A_MQTT_SubInfo.ip_addrlh, A_MQTT_SubInfo.ip_addrll);
		A_MQTT_SubInfo.mqtt_err_c = mqtt_client_connect(mqtt_client, &Subscriber_ip, MQTT_PORT, mqtt_client_connection_callback, 0, &mqtt_client_info);
	}
}
#endif

