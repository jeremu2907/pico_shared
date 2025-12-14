#pragma once

// NO_SYS = 1 for polling/bare-metal mode
#define NO_SYS 1

// Disable socket and netconn APIs (not available in NO_SYS mode)
#define LWIP_SOCKET 0
#define LWIP_NETCONN 0

// Enable IPv4
#define LWIP_IPV4 1

// DHCP Server (needed for AP mode)
#define LWIP_DHCP 1
#define LWIP_DHCPS 1

// DNS
#define LWIP_DNS 1

// Enable ARP
#define LWIP_ARP 1

// Enable UDP (needed for DHCP server)
#define LWIP_UDP 1

// Enable TCP
#define LWIP_TCP 1

// Memory settings
// #define MEM_LIBC_MALLOC 1
#define MEMP_NUM_TCP_PCB 8
#define MEMP_NUM_TCP_PCB_LISTEN 8
#define MEMP_NUM_UDP_PCB 8
#define PBUF_POOL_SIZE 24

// TCP settings
#define TCP_MSS 1460
#define TCP_WND (8 * TCP_MSS)
#define TCP_SND_BUF (8 * TCP_MSS)
#define TCP_SND_QUEUELEN ((4 * (TCP_SND_BUF) + (TCP_MSS - 1)) / (TCP_MSS))
#define MEMP_NUM_TCP_SEG TCP_SND_QUEUELEN

// Enable IGMP (multicast)
#define LWIP_IGMP 1

// DHCP server settings for AP mode
#define DHCP_DOES_ARP_CHECK 0