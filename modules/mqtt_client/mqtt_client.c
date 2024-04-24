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
 *  Created on: Apr 24, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"

#ifdef MQTT_ENABLE
#include "../../kernel/kernel_opt.h"

#include "../../modules/lwip2.2/LwIp/src/include/lwip/apps/mqtt.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

extern	Modules_t		Modules[MODULES_NUM];
extern	Asys_t			Asys;

SYSTEM_RAM	static	A_MQTT_SubInfo_t	A_MQTT_SubInfo;
SYSTEM_RAM	static 	ip_addr_t			mqtt_ip;
SYSTEM_RAM	static 	mqtt_client_t		*mqtt_client;

static struct mqtt_connect_client_info_t mqtt_client_info=
{
	"test",
	NULL, /* user */
	NULL, /* pass */
	100,  /* keep alive */
	NULL, /* will_topic */
	NULL, /* will_msg */
	0,    /* will_qos */
	0     /* will_retain */
#if LWIP_ALTCP && LWIP_ALTCP_TLS
	, NULL
#endif
};

ITCM_AREA_CODE static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
//const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;
	memcpy(A_MQTT_SubInfo.mqtt_incoming_data_ptr,data,len);
	if (( Modules[MODULE_IDX_MQTT].status & MODULE_STATUS_ALLOCATED ) == MODULE_STATUS_ALLOCATED )
		activate_process(Modules[MODULE_IDX_MQTT].process,WAKEUP_FROM_SW_MODULES_IRQ,MODULE_MQTT);
}

ITCM_AREA_CODE static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
//const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;
}

ITCM_AREA_CODE static void mqtt_request_cb(void *arg, err_t err)
{
//const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;
}

ITCM_AREA_CODE static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;
	if (status == MQTT_CONNECT_ACCEPTED)
	{
		mqtt_sub_unsub(client,A_MQTT_SubInfo.topic, A_MQTT_SubInfo.qos,mqtt_request_cb, LWIP_CONST_CAST(void*, client_info),1);
	}
}

ITCM_AREA_CODE uint8_t mqtt_client_init(uint8_t *broker_ip_addr,char *topic,char *client_identity, char *client_user, char *client_pass, char *mqtt_incoming_data_ptr)
{
	mqtt_client = mqtt_client_new();
	memset(&A_MQTT_SubInfo, 0, sizeof(A_MQTT_SubInfo));
	A_MQTT_SubInfo.ip_addrhh = broker_ip_addr[0];
	A_MQTT_SubInfo.ip_addrhl = broker_ip_addr[1];
	A_MQTT_SubInfo.ip_addrlh = broker_ip_addr[2];
	A_MQTT_SubInfo.ip_addrll = broker_ip_addr[3];
	strcpy(A_MQTT_SubInfo.topic,topic);
	strcpy(A_MQTT_SubInfo.client_identity,client_identity);
	if ( client_user != NULL )
		strcpy(A_MQTT_SubInfo.client_user,client_user);
	if ( client_pass != NULL )
		strcpy(A_MQTT_SubInfo.client_pass,client_pass);
	A_MQTT_SubInfo.mqtt_err_c = 0;
	A_MQTT_SubInfo.qos = 0;
	A_MQTT_SubInfo.mqtt_incoming_data_ptr = mqtt_incoming_data_ptr;
	A_MQTT_SubInfo.max_collisions = MAX_COLLISIONS;

	mqtt_client_info.client_id = A_MQTT_SubInfo.client_identity;
	if ( client_user != NULL )
		mqtt_client_info.client_user = A_MQTT_SubInfo.client_user;
	if ( client_pass != NULL )
		mqtt_client_info.client_pass = A_MQTT_SubInfo.client_pass;
	mqtt_client_info.keep_alive = 100;
	mqtt_client_info.will_qos = 0;
	mqtt_client_info.will_retain = 0;
	mqtt_client_info.will_topic = A_MQTT_SubInfo.topic;

	IP4_ADDR(&mqtt_ip, A_MQTT_SubInfo.ip_addrhh, A_MQTT_SubInfo.ip_addrhl, A_MQTT_SubInfo.ip_addrlh, A_MQTT_SubInfo.ip_addrll);
	mqtt_set_inpub_callback(mqtt_client,mqtt_incoming_publish_cb,mqtt_incoming_data_cb,LWIP_CONST_CAST(void*, &mqtt_client_info));
	mqtt_client_connect(mqtt_client,&mqtt_ip, MQTT_PORT,mqtt_connection_cb, LWIP_CONST_CAST(void*, &mqtt_client_info),&mqtt_client_info);
	return 0;
}

ITCM_AREA_CODE void mqtt_client_set_qos(uint8_t qos)
{
	if ( qos )
		A_MQTT_SubInfo.qos = 1;
	else
		A_MQTT_SubInfo.qos = 0;
}

ITCM_AREA_CODE uint32_t mqtt_client_check_connect(void)
{
	if ( mqtt_client_is_connected(mqtt_client) != 1 )
		mqtt_client_connect(mqtt_client,&mqtt_ip, MQTT_PORT,mqtt_connection_cb, LWIP_CONST_CAST(void*, &mqtt_client_info),&mqtt_client_info);
	return 0;
}

ITCM_AREA_CODE uint32_t mqtt_client_send(char *topic, char *message,uint32_t message_len)
{
char	arg[32] = "mqtt_arg";
int8_t	err;

	if ( mqtt_client_is_connected(mqtt_client) != 1 )
		mqtt_client_check_connect();

	err = mqtt_publish(mqtt_client, topic, message, strlen(message), 0, 0, (mqtt_request_cb_t )mqtt_incoming_publish_cb, &arg);
	if ( err != 0 )
		message_len = 0;
	return message_len;
}

#endif // #ifdef MQTT_ENABLE

