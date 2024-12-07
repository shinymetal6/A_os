/*
 * mqtt_client.c
 *
 *  Created on: May 13, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"

#ifdef MQTT_ENABLE
//#include "../../kernel/kernel_opt.h"

#include "../../modules/lwip2.2/LwIp/src/include/lwip/apps/mqtt.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

extern	Modules_t		Modules[MODULES_NUM];
extern	Asys_t			Asys;

SYSTEM_RAM	static	A_MQTT_SubInfo_t	A_MQTT_SubInfo;
SYSTEM_RAM	static	mqtt_client_t		*mqtt_client;
SYSTEM_RAM	static	ip_addr_t			mqtt_ip;

static const struct mqtt_connect_client_info_t mqtt_client_info =
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

static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;
	LWIP_UNUSED_ARG(data);
	LWIP_PLATFORM_DIAG(("MQTT client \"%s\" data cb: len %d, flags %d\n", client_info->client_id, (int)len, (int)flags));
	activate_process(Modules[MODULE_IDX_MQTT].process,WAKEUP_FROM_SW_MODULES_IRQ,MODULE_MQTT_RXFLAG);
}

static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;

	LWIP_PLATFORM_DIAG(("MQTT client \"%s\" publish cb: topic %s, len %d\n", client_info->client_id, topic, (int)tot_len));
}

static void mqtt_request_cb(void *arg, err_t err)
{
const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;

	LWIP_PLATFORM_DIAG(("MQTT client \"%s\" request cb: err %d\n", client_info->client_id, (int)err));
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
uint8_t	i;
const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;
	LWIP_UNUSED_ARG(client);

	LWIP_PLATFORM_DIAG(("MQTT client \"%s\" connection cb: status %d\n", client_info->client_id, (int)status));

	if (status == MQTT_CONNECT_ACCEPTED)
	{
		A_MQTT_SubInfo.connected |= MQTT_CONNECTED;
		for(i=0;i<A_MQTT_SubInfo.topic_index;i++)
			mqtt_sub_unsub(client,A_MQTT_SubInfo.topics[i],A_MQTT_SubInfo.qos,mqtt_request_cb, LWIP_CONST_CAST(void*, client_info),1);
	}
}

uint32_t mqtt_client_check_connect(void)
{
	if ( mqtt_client_is_connected(mqtt_client) != 1 )
		mqtt_client_connect(mqtt_client,&mqtt_ip, MQTT_PORT,mqtt_connection_cb, LWIP_CONST_CAST(void*, &mqtt_client_info),&mqtt_client_info);
	return 0;
}

uint32_t mqtt_client_send(char *topic, char *message,uint32_t message_len)
{
char	arg[32] = "mqtt_arg";
int8_t	err;

	if ( mqtt_client_is_connected(mqtt_client) != 1 )
		mqtt_client_check_connect();

	err = mqtt_publish(mqtt_client, topic, message, strlen(message), 0, 0, (mqtt_request_cb_t )mqtt_incoming_publish_cb, &arg);
	if ( err != 0 )
		return 0;
	return message_len;
}

uint8_t mqtt_client_add_subscribed_topic(char *topic)
{
uint8_t	ret_val = 1;

	if ( A_MQTT_SubInfo.topic_index < (MQTT_MAX_TOPICS-1))
	{
		strcpy(A_MQTT_SubInfo.topics[A_MQTT_SubInfo.topic_index],topic);
		A_MQTT_SubInfo.topic_index++;
		mqtt_sub_unsub(mqtt_client,topic,A_MQTT_SubInfo.qos,mqtt_request_cb, LWIP_CONST_CAST(void*, &mqtt_client_info),1);
		ret_val = 0;
	}
	return ret_val;
}

uint8_t mqtt_client_remove_subscribed_topic(char *topic)
{
uint8_t	ret_val = 1 , i,found=0, scan_idx=0;

	mqtt_sub_unsub(mqtt_client,topic,A_MQTT_SubInfo.qos,mqtt_request_cb, LWIP_CONST_CAST(void*, &mqtt_client_info),0);

	bzero(A_MQTT_SubInfo.tmp_topics,MQTT_MAX_TOPICS*MQTT_MAX_TOPIC_CHARS);
	for(i=0;i<A_MQTT_SubInfo.topic_index;i++)
	{
		if ( strcmp(A_MQTT_SubInfo.topics[i],topic) != 0)
		{
			strcpy(A_MQTT_SubInfo.tmp_topics[scan_idx] , A_MQTT_SubInfo.topics[i]);
			scan_idx++;
		}
		else
			found = 1;

	}
	if ( found )
	{
		A_MQTT_SubInfo.topic_index -= 1;
		bzero(A_MQTT_SubInfo.topics,MQTT_MAX_TOPICS*MQTT_MAX_TOPIC_CHARS);
		memcpy(A_MQTT_SubInfo.topics,A_MQTT_SubInfo.tmp_topics,MQTT_MAX_TOPICS*MQTT_MAX_TOPIC_CHARS);
		ret_val = 0;
	}
	return ret_val;
}

uint8_t mqtt_client_init(uint8_t *broker_ip_addr,char *main_topic,char *client_identity, char *client_user, char *client_pass, char *mqtt_incoming_data_ptr)
{
	mqtt_client = mqtt_client_new();
	A_MQTT_SubInfo.ip_addrhh = broker_ip_addr[0];
	A_MQTT_SubInfo.ip_addrhl = broker_ip_addr[1];
	A_MQTT_SubInfo.ip_addrlh = broker_ip_addr[2];
	A_MQTT_SubInfo.ip_addrll = broker_ip_addr[3];
	IP4_ADDR(&mqtt_ip, A_MQTT_SubInfo.ip_addrhh, A_MQTT_SubInfo.ip_addrhl, A_MQTT_SubInfo.ip_addrlh, A_MQTT_SubInfo.ip_addrll);
	A_MQTT_SubInfo.qos = 0;
	A_MQTT_SubInfo.mqtt_incoming_data_ptr = mqtt_incoming_data_ptr;

	strcpy(A_MQTT_SubInfo.topics[0],main_topic);
	A_MQTT_SubInfo.topic_index=1;

	mqtt_set_inpub_callback(mqtt_client,mqtt_incoming_publish_cb,mqtt_incoming_data_cb,LWIP_CONST_CAST(void*, &mqtt_client_info));
	mqtt_client_connect(mqtt_client,&mqtt_ip, MQTT_PORT,mqtt_connection_cb, LWIP_CONST_CAST(void*, &mqtt_client_info),&mqtt_client_info);

  return 0;
}


#endif
