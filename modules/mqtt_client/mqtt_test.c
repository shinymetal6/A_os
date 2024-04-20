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
 * mqtt_test.c
 *
 *  Created on: Apr 19, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"

#ifdef MQTT_ENABLE
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "../../modules/lwip2.2/LwIp/src/include/lwip/apps/mqtt.h"

#define IP_ADDR0 192
#define IP_ADDR1 168
#define IP_ADDR2 10
#define IP_ADDR3 204
#define SUB_TOPIC "lwip/sub"
#define PUB_TOPIC "lwip/pub"
#define FLAG_SUB 0
#define FLAG_PUB 1
#define LWIP_DEBUG

/* The idea is to demultiplex topic and create some reference to be used in data callbacks
   Example here uses a global variable, better would be to use a member in arg
   If RAM and CPU budget allows it, the easiest implementation might be to just take a copy of
   the topic string and use it in mqtt_incoming_data_cb
*/
static int inpub_id;
static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
  printf("Incoming publish at topic %s with total length %u\n", topic, (unsigned int)tot_len);

  /* Decode topic string into a user defined reference */
  if (strcmp(topic, "print_payload") == 0)
  {
    inpub_id = 0;
  }
  else if (topic[0] == 'A')
  {
    /* All topics starting with 'A' might be handled at the same way */
    inpub_id = 1;
  }
  else
  {
    /* For all other topics */
    inpub_id = 2;
  }
  inpub_id = -1;
}

static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
  printf("Incoming publish payload with length %d, flags %u\n", len, (unsigned int)flags);

  printf("mqtt payload: %s\n", (const char *)data);
  if (flags & MQTT_DATA_FLAG_LAST)
  {
    /* Last fragment of payload received (or whole part if payload fits receive buffer
       See MQTT_VAR_HEADER_BUFFER_LEN)  */
    /* Call function or do action depending on reference, in this case inpub_id */
    if (inpub_id == -1)
    {
      return;
    }
    else if (inpub_id == 0)
    {
      /* Don't trust the publisher, check zero termination */
      if (data[len - 1] == 0)
      {
        printf("mqtt_incoming_data_cb: %s\n", (const char *)data);
      }
    }
    else if (inpub_id == 1)
    {
      /* Call an 'A' function... */
    }
    else
    {
      printf("mqtt_incoming_data_cb: Ignoring payload...\n");
    }
  }
  else
  {
    /* Handle fragmented payload, store in buffer, write to file or whatever */
  }
}

static void mqtt_sub_request_cb(void *arg, err_t result)
{
  /* Just print the result code here for simplicity,
     normal behaviour would be to take some action if subscribe fails like
     notifying user, retry subscribe or disconnect from server */
  printf("Subscribe result: %d\n", result);
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
  err_t err;
  if (status == MQTT_CONNECT_ACCEPTED)
  {
    printf("mqtt_connection_cb: Successfully connected\n");

    /* Setup callback for incoming publish requests */
    mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, arg);

    /* Subscribe to a topic named "subtopic" with QoS level 1, call mqtt_sub_request_cb with result */
    err = mqtt_subscribe(client, SUB_TOPIC, 1, mqtt_sub_request_cb, arg);

    if (err != ERR_OK)
    {
      printf("mqtt_subscribe return: %d\n", err);
    }
  }
  else
  {
    printf("mqtt_connection_cb: Disconnected, reason: %d\n", status);

    /* Its more nice to be connected, so try to reconnect */
    mqtt_client_do_connect(client, FLAG_SUB);
  }
}

/* Called when publish is complete either with sucess or failure */
static void mqtt_pub_request_cb(void *arg, err_t result)
{
  if (result != ERR_OK)
  {
    printf("Publish result: %d\n", result);
  }
}

static void mqtt_pub_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
  err_t err;
  if (status == MQTT_CONNECT_ACCEPTED)
  {
    printf("mqtt_connection_cb: Successfully connected\n");

    const char *pub_payload = "hello this is lwIP";
    err_t err;
    u8_t qos = 2;    /* 0 1 or 2, see MQTT specification */
    u8_t retain = 0; /* No don't retain such crappy payload... */
    err = mqtt_publish(client, PUB_TOPIC, pub_payload, strlen(pub_payload), qos, retain, mqtt_pub_request_cb, arg);
    if (err != ERR_OK)
    {
      printf("Publish err: %d\n", err);
    }
  }
  else
  {
    printf("mqtt_connection_cb: Disconnected, reason: %d\n", status);
  }
}

void mqtt_client_do_connect(mqtt_client_t *client, int flag)
{
  struct mqtt_connect_client_info_t ci;
  err_t err;

  /* Setup an empty client info structure */
  memset(&ci, 0, sizeof(ci));

  /* Minimal amount of information required is client identifier, so set it here */
  ci.client_id = "lwip_test";

  /* Initiate client and connect to server, if this fails immediately an error code is returned
     otherwise mqtt_connection_cb will be called with connection result after attempting
     to establish a connection with the server.
     For now MQTT version 3.1.1 is always used */

  ip_addr_t ip_addr;
  IP4_ADDR(&ip_addr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);

  if (flag == FLAG_SUB)
  {
    err = mqtt_client_connect(client, &ip_addr, 1883, mqtt_connection_cb, NULL, &ci);
  }
  else if (flag == FLAG_PUB)
  {
    err = mqtt_client_connect(client, &ip_addr, 1883, mqtt_pub_connection_cb, NULL, &ci);
  }

  /* For now just print the result code if something goes wrong */
  if (err != ERR_OK)
  {
    printf("mqtt_connect return %d\n", err);
  }
}

int mqttClient(char *argv)
{

  if (strcmp(argv, "sub") == 0)
  {
    // do sub
    mqtt_client_t *client = mqtt_client_new();
    if (client != NULL)
    {
      mqtt_client_do_connect(client, FLAG_SUB);
    }
  }
  else if (strcmp(argv, "pub") == 0)
  {
    // do pub
    mqtt_client_t *clientpub = mqtt_client_new();
    if (clientpub != NULL)
    {
      mqtt_client_do_connect(clientpub, FLAG_PUB);
    }
  }
  return 0;
}

#endif // #ifdef MQTT_ENABLE

