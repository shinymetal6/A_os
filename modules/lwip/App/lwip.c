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
 * lwip.c
 *
 *  Created on: Apr 13, 2024
 *      Author: fil
 */

#include "../../../../A_os/kernel/A.h"

#ifdef	NETWORKING_ENABLED
#include <string.h>
#include "lwip.h"
#include "../LwIp/src/include/lwip/init.h"
#include "../LwIp/src/include/lwip/netif.h"
#include "../Target/ethernetif.h"
#ifndef A_USES_DHCP
	//#undef 	LWIP_DHCP
#endif // #ifdef NETWORKING_ENABLED

ETH_DATA_AREA	ETH_DMADescTypeDef DMARxDscrTab[32] ; /* Ethernet Rx DMA Descriptors */
ETH_DATA_AREA	ETH_DMADescTypeDef DMATxDscrTab[32];   /* Ethernet Tx DMA Descriptors */

static void ethernet_link_status_updated(struct netif *netif);
static void Ethernet_Link_Periodic_Handle(struct netif *netif);
void Error_Handler(void);

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
	memset(&DMARxDscrTab, 0, sizeof(DMARxDscrTab));
	memset(&DMATxDscrTab, 0, sizeof(DMARxDscrTab));
	memset(&gnetif, 0, sizeof(gnetif));

	lwip_init();

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

	netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

	netif_set_default(&gnetif);

	if (netif_is_link_up(&gnetif))
	{
		netif_set_up(&gnetif);
	}
	else
	{
		netif_set_down(&gnetif);
	}

	netif_set_link_callback(&gnetif, ethernet_link_status_updated);
}

extern	void httpd_init(void);

void MX_LWIP_HttpdStart(void)
{
	httpd_init();
	HAL_Delay(1);
}

extern	A_IpAddr_t	A_DhcpIpAddr;
extern	Asys_t			Asys;

#define	LWIP_DHCP_INITIAL		10
#define	LWIP_DHCP_MAX_INTERVAL	160
uint8_t	lwip_dhcp_tim=0;
uint8_t	lwip_dhcp_interval=LWIP_DHCP_MAX_INTERVAL/8;
uint8_t	lwip_dhcp_err=0;
static void Ethernet_Link_Periodic_Handle(struct netif *netif)
{
	/* Ethernet Link every 100ms */
	if ((Asys.g_tick_count - EthernetLinkTimer) >= 100)
	{
		EthernetLinkTimer = Asys.g_tick_count;
		ethernet_link_check_state(netif);
#if LWIP_DHCP
		if (netif_is_link_up(netif))
		{
			if ( dhcp_supplied_address(netif) != 1 )
			{
				if ( lwip_dhcp_tim > lwip_dhcp_interval )
				{
					lwip_dhcp_tim = 0;
					lwip_dhcp_interval *=2;
					if ( lwip_dhcp_interval > LWIP_DHCP_MAX_INTERVAL )
						lwip_dhcp_interval = LWIP_DHCP_INITIAL;
					if ( dhcp_start(&gnetif) != 0 )
					{
						lwip_dhcp_err++;
						return;
					}
				}
				else
					lwip_dhcp_tim ++;
			}
			else
			{
				A_DhcpIpAddr.GW_ADDR0 = netif->gw.addr >> 24;
				A_DhcpIpAddr.GW_ADDR1 = netif->gw.addr >> 16;
				A_DhcpIpAddr.GW_ADDR2 = netif->gw.addr >> 8;
				A_DhcpIpAddr.GW_ADDR3 = netif->gw.addr >> 0;
				A_DhcpIpAddr.IP_ADDR0 = netif->ip_addr.addr >> 24;
				A_DhcpIpAddr.IP_ADDR1 = netif->ip_addr.addr >> 16;
				A_DhcpIpAddr.IP_ADDR2 = netif->ip_addr.addr >> 8;
				A_DhcpIpAddr.IP_ADDR3 = netif->ip_addr.addr >> 0;
			}
		}
#endif
	}
}

uint8_t MX_is_network_up(void)
{
	return dhcp_supplied_address(&gnetif);
}

void MX_LWIP_Process(void)
{
	ethernetif_input(&gnetif);
	/* Handle timeouts */
	sys_check_timeouts();
	Ethernet_Link_Periodic_Handle(&gnetif);
}

static void ethernet_link_status_updated(struct netif *netif)
{
	//__disable_irq();
	if (netif_is_up(netif))
	{
	}
	else /* netif is down */
	{
	}
	//__enable_irq();
}

#endif	//#ifdef	NETWORKING_ENABLED

