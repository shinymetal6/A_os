/*
 * mqtt_client.c
 *
 *  Created on: May 13, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"

#ifdef MQTT_ENABLE

#include "../../libraries/lwip2.2/LwIp/src/include/lwip/apps/mqtt.h"
#include "mqtt_client.h"
#include <string.h>

extern	Modules_t		Modules[MODULES_NUM];
extern	Asys_t			Asys;

SYSTEM_RAM	static	A_Mqtt_TypeDef		A_Mqtt;
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

ITCM_AREA_CODE static uint32_t get_handle_from_module(uint8_t module_id)
{
uint32_t	i,drv_ret=255;
	for(i=0;i<MODULES_NUM;i++)
		if ( MODULES_Struct[i].handle ==  module_id)
			if ( MODULES_Struct[i].private_data != NULL )
					return i;
	return drv_ret;
}

ITCM_AREA_CODE static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
#ifdef MQTT_MODULE_DEBUG
const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;
#endif
uint32_t handle;
LWIP_UNUSED_ARG(data);

	if ( (handle = get_handle_from_module(MODULE_ID_MQTT)) != 255 )
	{
#ifdef MQTT_MODULE_DEBUG
		LWIP_PLATFORM_DIAG(("MQTT client \"%s\" data cb: len %d, flags %d\n", client_info->client_id, (int)len, (int)flags));
#endif
		activate_process(MODULES_Struct[handle].process ,WAKEUP_FROM_SW_MODULES_IRQ,MODULE_MQTT_RXFLAG);
	}
}

ITCM_AREA_CODE static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;

	LWIP_PLATFORM_DIAG(("MQTT client \"%s\" publish cb: topic %s, len %d\n", client_info->client_id, topic, (int)tot_len));
}

ITCM_AREA_CODE static void mqtt_request_cb(void *arg, err_t err)
{
const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;

	LWIP_PLATFORM_DIAG(("MQTT client \"%s\" request cb: err %d\n", client_info->client_id, (int)err));
}

ITCM_AREA_CODE static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
uint8_t	i;
const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;
	LWIP_UNUSED_ARG(client);

	LWIP_PLATFORM_DIAG(("MQTT client \"%s\" connection cb: status %d\n", client_info->client_id, (int)status));

	if (status == MQTT_CONNECT_ACCEPTED)
	{
		A_Mqtt.connected |= MQTT_CONNECTED;
		for(i=0;i<A_Mqtt.topic_index;i++)
			mqtt_sub_unsub(client,A_Mqtt.topics[i],A_Mqtt.qos,mqtt_request_cb, LWIP_CONST_CAST(void*, client_info),1);
	}
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
		return 0;
	return message_len;
}

ITCM_AREA_CODE uint8_t mqtt_client_add_subscribed_topic(char *topic)
{
uint8_t	ret_val = 1;

	if ( A_Mqtt.topic_index < (MQTT_MAX_TOPICS-1))
	{
		strcpy(A_Mqtt.topics[A_Mqtt.topic_index],topic);
		A_Mqtt.topic_index++;
		mqtt_sub_unsub(mqtt_client,topic,A_Mqtt.qos,mqtt_request_cb, LWIP_CONST_CAST(void*, &mqtt_client_info),1);
		ret_val = 0;
	}
	return ret_val;
}

ITCM_AREA_CODE uint8_t mqtt_client_remove_subscribed_topic(char *topic)
{
uint8_t	ret_val = 1 , i,found=0, scan_idx=0;

	mqtt_sub_unsub(mqtt_client,topic,A_Mqtt.qos,mqtt_request_cb, LWIP_CONST_CAST(void*, &mqtt_client_info),0);

	bzero(A_Mqtt.tmp_topics,MQTT_MAX_TOPICS*MQTT_MAX_TOPIC_CHARS);
	for(i=0;i<A_Mqtt.topic_index;i++)
	{
		if ( strcmp(A_Mqtt.topics[i],topic) != 0)
		{
			strcpy(A_Mqtt.tmp_topics[scan_idx] , A_Mqtt.topics[i]);
			scan_idx++;
		}
		else
			found = 1;

	}
	if ( found )
	{
		A_Mqtt.topic_index -= 1;
		bzero(A_Mqtt.topics,MQTT_MAX_TOPICS*MQTT_MAX_TOPIC_CHARS);
		memcpy(A_Mqtt.topics,A_Mqtt.tmp_topics,MQTT_MAX_TOPICS*MQTT_MAX_TOPIC_CHARS);
		ret_val = 0;
	}
	return ret_val;
}

/*
uint8_t mqtt_client_init(uint8_t *broker_ip_addr,char *main_topic,char *client_identity, char *client_user, char *client_pass, char *mqtt_incoming_data_ptr)
{
	mqtt_client = mqtt_client_new();
	A_Mqtt.ip_addrhh = broker_ip_addr[0];
	A_Mqtt.ip_addrhl = broker_ip_addr[1];
	A_Mqtt.ip_addrlh = broker_ip_addr[2];
	A_Mqtt.ip_addrll = broker_ip_addr[3];
	IP4_ADDR(&mqtt_ip, A_Mqtt.ip_addrhh, A_Mqtt.ip_addrhl, A_Mqtt.ip_addrlh, A_Mqtt.ip_addrll);
	A_Mqtt.qos = 0;
	A_Mqtt.mqtt_incoming_data_ptr = mqtt_incoming_data_ptr;

	strcpy(A_Mqtt.topics[0],main_topic);
	A_Mqtt.topic_index=1;

	mqtt_set_inpub_callback(mqtt_client,mqtt_incoming_publish_cb,mqtt_incoming_data_cb,LWIP_CONST_CAST(void*, &mqtt_client_info));
	mqtt_client_connect(mqtt_client,&mqtt_ip, MQTT_PORT,mqtt_connection_cb, LWIP_CONST_CAST(void*, &mqtt_client_info),&mqtt_client_info);

  return 0;
}
*/
ITCM_AREA_CODE static uint32_t	mqtt_client_check_private_data(A_Mqtt_TypeDef *mqtt_module)
{
	if (( mqtt_module->broker_ip_addr[3] == 0 ) && ( mqtt_module->broker_ip_addr[2] == 0 ) && ( mqtt_module->broker_ip_addr[1] == 0 ) && ( mqtt_module->broker_ip_addr[0] == 0 ))
		return 1;
	if ( mqtt_module->mqtt_client == NULL )
		return 1;
	return 0;
}

ITCM_AREA_CODE uint32_t	mqtt_client_register(A_Mqtt_TypeDef *private_data)
{
A_Mqtt_TypeDef	*mqtt_module;
	if ( MODULES_Struct[last_module_used_handle].process == 0 )
	{
		MODULES_Struct[last_module_used_handle].handle = MODULE_ID_MQTT;
		MODULES_Struct[last_module_used_handle].process = get_current_process();
		MODULES_Struct[last_module_used_handle].private_data = (uint32_t *)private_data;

		mqtt_module = (A_Mqtt_TypeDef *)MODULES_Struct[last_module_used_handle].private_data;
		mqtt_module->mqtt_client = mqtt_client_new();
		if ( mqtt_client_check_private_data(mqtt_module) == 0 )
		{
			IP4_ADDR(&mqtt_module->mqtt_ip, mqtt_module->broker_ip_addr[3], mqtt_module->broker_ip_addr[2], mqtt_module->broker_ip_addr[1], mqtt_module->broker_ip_addr[0]);
			A_Mqtt.qos = 0;
			mqtt_set_inpub_callback(mqtt_module->mqtt_client,mqtt_incoming_publish_cb,mqtt_incoming_data_cb,LWIP_CONST_CAST(void*, &mqtt_client_info));
			mqtt_client_connect(mqtt_module->mqtt_client,&mqtt_module->mqtt_ip, MQTT_PORT,mqtt_connection_cb, LWIP_CONST_CAST(void*, &mqtt_client_info),&mqtt_client_info);
			return 0;
		}
	}
	return 1;
}
#endif
