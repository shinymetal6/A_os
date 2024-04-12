/* USER CODE BEGIN Header */
/**
 ******************************************************************************
  * File Name          : LWIP.c
  * Description        : This file provides initialization code for LWIP
  *                      middleWare.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "lwip.h"
#include "../LwIp/src/include/lwip/init.h"
#include "../LwIp/src/include/lwip/netif.h"
#include "../Target/ethernetif.h"
#include "../../../../A_os/kernel/A.h"

#ifdef	STM32H743xx
//ETH_RX_DESC_CNT
ETH_DATA_AREA	ETH_DMADescTypeDef DMARxDscrTab[32] ; /* Ethernet Rx DMA Descriptors */
ETH_DATA_AREA	ETH_DMADescTypeDef DMATxDscrTab[32];   /* Ethernet Tx DMA Descriptors */

static void ethernet_link_status_updated(struct netif *netif);
static void Ethernet_Link_Periodic_Handle(struct netif *netif);
void Error_Handler(void);

/* DHCP Variables initialization ---------------------------------------------*/
uint32_t DHCPfineTimer = 0;
uint32_t DHCPcoarseTimer = 0;
uint32_t EthernetLinkTimer;

struct netif gnetif;
ip4_addr_t ipaddr;
ip4_addr_t netmask;
ip4_addr_t gw;

extern	err_t ethernet_input(struct pbuf *p, struct netif *netif);

void MX_LWIP_Init(A_IpAddr_t *A_IpAddr)
{
  /* Initilialize the LwIP stack without RTOS */
  lwip_init();

  /* IP addresses initialization with DHCP (IPv4) */
  ipaddr.addr = 0;
  netmask.addr = 0;
  gw.addr = 0;

#if LWIP_DHCP
  ip_addr_set_zero_ip4(&ipaddr);
  ip_addr_set_zero_ip4(&netmask);
  ip_addr_set_zero_ip4(&gw);
#else

  /* IP address default setting */
  IP4_ADDR(&ipaddr, A_IpAddr->IP_ADDR0, A_IpAddr->IP_ADDR1, A_IpAddr->IP_ADDR2, A_IpAddr->IP_ADDR3);
  IP4_ADDR(&netmask, A_IpAddr->NETMASK_ADDR0, A_IpAddr->NETMASK_ADDR1 , A_IpAddr->NETMASK_ADDR2, A_IpAddr->NETMASK_ADDR3);
  IP4_ADDR(&gw, A_IpAddr->GW_ADDR0, A_IpAddr->GW_ADDR1, A_IpAddr->GW_ADDR2, A_IpAddr->GW_ADDR3);

#endif

  /* add the network interface (IPv4/IPv6) without RTOS */
  netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

  /* Registers the default network interface */
  netif_set_default(&gnetif);

  if (netif_is_link_up(&gnetif))
  {
    /* When the netif is fully configured this function must be called */
    netif_set_up(&gnetif);
  }
  else
  {
    /* When the netif link is down this function must be called */
    netif_set_down(&gnetif);
  }

  /* Set the link callback function, this function is called on change of link status*/
  netif_set_link_callback(&gnetif, ethernet_link_status_updated);

  /* Create the Ethernet link handler thread */

  /* Start DHCP negotiation for a network interface (IPv4) */
#if LWIP_DHCP
  dhcp_start(&gnetif);
#endif
}

extern	A_IpAddr_t	A_DhcpIpAddr;
extern	Asys_t			Asys;

static void Ethernet_Link_Periodic_Handle(struct netif *netif)
{
  /* Ethernet Link every 100ms */
	//if (HAL_GetTick() - EthernetLinkTimer >= 100)
	if (Asys.g_os_started )
	{
		  if ((Asys.g_tick_count - EthernetLinkTimer) >= 100)
		  {
		    //EthernetLinkTimer = HAL_GetTick();
		    EthernetLinkTimer = Asys.g_tick_count;
		    ethernet_link_check_state(netif);
		#if LWIP_DHCP
		    A_DhcpIpAddr.GW_ADDR0 = netif->gw.addr >> 24;
		    A_DhcpIpAddr.GW_ADDR1 = netif->gw.addr >> 16;
		    A_DhcpIpAddr.GW_ADDR2 = netif->gw.addr >> 8;
		    A_DhcpIpAddr.GW_ADDR3 = netif->gw.addr >> 0;
		    A_DhcpIpAddr.IP_ADDR0 = netif->ip_addr.addr >> 24;
		    A_DhcpIpAddr.IP_ADDR1 = netif->ip_addr.addr >> 16;
		    A_DhcpIpAddr.IP_ADDR2 = netif->ip_addr.addr >> 8;
		    A_DhcpIpAddr.IP_ADDR3 = netif->ip_addr.addr >> 0;
		#endif
	}
  }
}

/**
 * ----------------------------------------------------------------------
 * Function given to help user to continue LwIP Initialization
 * Up to user to complete or change this function ...
 * Up to user to call this function in main.c in while (1) of main(void)
 *-----------------------------------------------------------------------
 * Read a received packet from the Ethernet buffers
 * Send it to the lwIP stack for handling
 * Handle timeouts if LWIP_TIMERS is set and without RTOS
 * Handle the llink status if LWIP_NETIF_LINK_CALLBACK is set and without RTOS
 */
void MX_LWIP_Process(void)
{
	if (Asys.g_os_started )
	{
		if ( gnetif.next == NULL )
		{
			ethernetif_input(&gnetif);
			/* Handle timeouts */
			__disable_irq();
			sys_check_timeouts();
			__enable_irq();
			Ethernet_Link_Periodic_Handle(&gnetif);
		}
	}
}

static void ethernet_link_status_updated(struct netif *netif)
{
  if (netif_is_up(netif))
  {
  }
  else /* netif is down */
  {
  }
}

#endif	//#ifdef	STM32H743xx


