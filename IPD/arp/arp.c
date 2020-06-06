//
// Created by hs on 2020/4/13.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <net/if_arp.h>
#include <linux/if_packet.h>

struct arppacket
{
    struct arphdr ar_head;
    unsigned char ar_sha[ETH_ALEN];
    struct in_addr ar_sip;
    unsigned char ar_tha[ETH_ALEN];
    struct in_addr ar_tip;
}__attribute__((packed));

void printMac(const char* p_head, const unsigned char* p_mac);

int main(int argc, char **argv)
{
    struct in_addr pingaddr;
    struct in_addr netaddr;
    struct in_addr netmask;
    struct sockaddr_ll hwaddr;
    memset(&netaddr, 0, sizeof(netaddr));
    memset(&netmask, 0, sizeof(netmask));
    memset(&hwaddr, 0, sizeof(hwaddr));

    if(argc != 2)
    {
        perror("Usage: arp 127.0.0.1\n");
        exit(-1);
    }

    if(inet_aton(argv[1], &pingaddr) < 0)
    {
        perror("not a correct ip address\n");
        exit(-1);
    }


    // create sock_packet socket
    int fd = socket(PF_PACKET, SOCK_RAW, htonl(ETH_P_ARP));
    if(fd == -1)
    {
        perror("create socket error\n");
        exit(-1);
    }


    // get net interface info
    char buf[1024];
    struct ifconf ifc;
    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    if(ioctl(fd, SIOCGIFCONF, &ifc) == -1)
    {
        perror("get net interface error\n");
        exit(-1);
    }


    // get correct net interface
    int flag = 0;
    struct ifreq *ifrPtr;
    int networkNum = ifc.ifc_len / sizeof(struct ifreq);
    for(int i = 0; i < networkNum; i++)
    {
        ifrPtr = ((struct ifreq*)buf) + i;
        if(ioctl(fd, SIOCGIFADDR, ifrPtr) == -1)
        {
            perror("get ip address error\n");
            exit(-1);
        }
        netaddr = ((struct sockaddr_in*)&(ifrPtr->ifr_addr))->sin_addr;

        if(ioctl(fd, SIOCGIFNETMASK, ifrPtr) == -1)
        {
            perror("get netmask error\n");
            exit(-1);
        }
        netmask = ((struct sockaddr_in*)&(ifrPtr->ifr_netmask))->sin_addr;

        if((pingaddr.s_addr & netmask.s_addr) == (netaddr.s_addr & netmask.s_addr))
        {
            hwaddr.sll_family = PF_PACKET;
            hwaddr.sll_protocol = htons(ETH_P_ARP);
            hwaddr.sll_hatype = ARPHRD_ETHER;
            hwaddr.sll_pkttype = PACKET_OTHERHOST;
            hwaddr.sll_halen = ETH_ALEN;

            if(ioctl(fd, SIOCGIFINDEX, ifrPtr) == -1)
            {
                perror("get net interface index error\n");
                exit(-1);
            }
            hwaddr.sll_ifindex = ifrPtr->ifr_ifindex;

            if(ioctl(fd, SIOCGIFHWADDR, ifrPtr) == -1)
            {
                perror("get net interface hwaddr error\n");
                exit(-1);
            }
            memcpy(hwaddr.sll_addr, ifrPtr->ifr_hwaddr.sa_data, ETH_ALEN);

            flag = 1;
            break;
        }
    }
    if(flag != 1)
    {
        perror("can not find correct net interface\n");
        exit(-1);
    }


    // send arp
    char ef[ETH_FRAME_LEN];
    struct ethhdr *p_eth = (struct ethhdr*)ef;
    memset(p_eth->h_dest, 0xff, ETH_ALEN);
    memcpy(p_eth->h_source, hwaddr.sll_addr, ETH_ALEN);
    p_eth->h_proto = htons(ETH_P_ARP);

    struct arppacket *p_arp = (struct arppacket*)(ef + ETH_HLEN);
    p_arp->ar_head.ar_hrd = htons(ARPHRD_ETHER);
    p_arp->ar_head.ar_pro = htons(ETH_P_IP);
    p_arp->ar_head.ar_hln = ETH_ALEN;
    p_arp->ar_head.ar_pln = 4;
    p_arp->ar_head.ar_op = htons(ARPOP_REQUEST);
    memcpy(p_arp->ar_sha, hwaddr.sll_addr, ETH_ALEN);
    p_arp->ar_sip = netaddr;
    memset(p_arp->ar_tha, 0, ETH_ALEN);
    p_arp->ar_tip = pingaddr;

    if(bind(fd, (struct sockaddr*)&hwaddr, sizeof(struct sockaddr_ll)) == -1)
    {
        perror("bind network error\n");
        exit(-1);
    }

    write(fd, ef, 60);
    while(1)
    {
        read(fd, ef, sizeof(ef));
        struct arppacket *recv_arp = (struct arppacket*)(ef+ETH_HLEN);
        if(recv_arp->ar_tip.s_addr == netaddr.s_addr)
        {
            printMac("mac: ", recv_arp->ar_sha);
            break;
        }
    }
	return 0;
}

void printMac(const char* p_head, const unsigned char* p_mac)
{
	printf("%s", p_head);
	for (int i = 0; i < ETH_ALEN - 1; i++)
		printf("%02x-", p_mac[i]);
	printf("%02x\n", p_mac[ETH_ALEN - 1]);
}
