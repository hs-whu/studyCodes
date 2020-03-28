#pragma pack (1)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <linux/if_packet.h>

#include "arp.h"

void printMac(const char* p_head, const unsigned char* p_mac);

int main()
{
	// 组织以太网帧
	char ef[ETH_FRAME_LEN];

	unsigned char eth_source[ETH_ALEN] = { 0x10, 0x7b, 0x44, 0xd8, 0xcd, 0x09 };
	unsigned char eth_dest[ETH_ALEN] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	unsigned char eth_null[ETH_ALEN] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	struct in_addr ip_source;
	struct in_addr ip_dest;
	inet_aton("192.168.100.9", &ip_source);
	inet_aton("192.168.100.254", &ip_dest);

	struct ethhdr* p_ethhdr;
	p_ethhdr = (struct ethhdr*)ef;
	memcpy(p_ethhdr->h_dest, eth_dest, ETH_ALEN);
	memcpy(p_ethhdr->h_source, eth_source, ETH_ALEN);
	p_ethhdr->h_proto = htons(ETH_P_ARP);

	struct arppacket* p_arp;
	p_arp = (struct arppacket*)(ef + ETH_HLEN);
	p_arp->ar_head.ar_hrd = htons(0x1);
	p_arp->ar_head.ar_pro = htons(ETH_P_IP);
	p_arp->ar_head.ar_hln = ETH_ALEN;
	p_arp->ar_head.ar_pln = 4;
	p_arp->ar_head.ar_op = htons(ARPOP_REQUEST);
	memcpy(p_arp->ar_sha, eth_source, ETH_ALEN);
	p_arp->ar_sip = ip_source;
	memcpy(p_arp->ar_tha, eth_null, ETH_ALEN);
	p_arp->ar_tip = ip_dest;

	// 注册原始套接字
	int fd;
	if ((fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) < 0)
	{
		perror("raw socket create error!\n");
		exit(-1);
	}

	// 获取设置网卡信息
	char* ethname = "enp2s0";
	struct ifreq ifr;
	strcpy(ifr.ifr_name, ethname);
	int i = ioctl(fd, SIOCGIFINDEX, &ifr);
	if (i < 0)
	{
		close(fd);
		perror("can't get index\n");
		exit(-1);
	}
	int index = ifr.ifr_ifindex;
	
	// 设置sockaddr_ll地址
	struct sockaddr_ll addr;
	addr.sll_family = AF_PACKET;
	addr.sll_protocol = htons(ETH_P_ARP);
	addr.sll_ifindex = index;
	addr.sll_hatype = ARPHRD_ETHER;
	addr.sll_pkttype = PACKET_OTHERHOST;
	addr.sll_halen = ETH_ALEN;
	memcpy(addr.sll_addr, eth_source, ETH_ALEN);
	
	// 绑定网卡
	int flag;
	if((flag = bind(fd, (struct sockaddr*)&addr, sizeof(addr))) == -1)
	{
		perror("bind fail\n");
		exit(-1);
	}

	// 发出以太网帧
	printf("**************************************************\n");
	int size = write(fd, ef, ETH_FRAME_LEN);
	printf("write size: %d\n", size);
	printMac("dest MAC: ", p_ethhdr->h_dest);

	printMac("source MAC: ", p_ethhdr->h_source);

	printf("protocol: 0x%04x\n\n", ntohs(p_ethhdr->h_proto));

	p_arp = (struct arppacket*)(ef + ETH_HLEN);
	printf("arp hard type: %d\n", ntohs(p_arp->ar_head.ar_hrd));
	printf("arp protocol: 0x%04x\n", ntohs(p_arp->ar_head.ar_pro));
	printf("arp mac length: %d\n", p_arp->ar_head.ar_hln);
	printf("arp ip length: %d\n", p_arp->ar_head.ar_pln);
	printf("arp op: %d\n", ntohs(p_arp->ar_head.ar_op));
	printMac("arp source MAC: ", p_arp->ar_sha);
	printf("arp source ip: %s\n", inet_ntoa(p_arp->ar_sip));
	printMac("arp dest MAC: ", p_arp->ar_tha);
	printf("arp dest ip: %s\n", inet_ntoa(p_arp->ar_tip));
	printf("**************************************************\n");

	// 设置网卡混杂模式
	/*i = ioctl(fd, SIOCGIFFLAGS, &ifr);
	if (i < 0)
	{
		close(fd);
		perror("can't get flags\n");
		exit(-1);
	}
	ifr.ifr_flags |= IFF_PROMISC;
	i = ioctl(fd, SIOCSIFFLAGS, &ifr);
	if (i < 0)
	{
		perror("promiscuous set error\n");
		exit(-1);
	}*/

	while(1)
	{
		printf("**************************************************\n");
		int size2 = read(fd, ef, ETH_FRAME_LEN);
		printf("read size: %d\n\n", size2);

		printMac("dest MAC: ", p_ethhdr->h_dest);

		printMac("source MAC: ", p_ethhdr->h_source);

		printf("protocol: 0x%04x\n\n", ntohs(p_ethhdr->h_proto));

		p_arp = (struct arppacket*)(ef + ETH_HLEN);
		printf("arp hard type: %d\n", ntohs(p_arp->ar_head.ar_hrd));
		printf("arp protocol: 0x%04x\n", ntohs(p_arp->ar_head.ar_pro));
		printf("arp mac length: %d\n", p_arp->ar_head.ar_hln);
		printf("arp ip length: %d\n", p_arp->ar_head.ar_pln);
		printf("arp op: %d\n", ntohs(p_arp->ar_head.ar_op));
		printMac("arp source MAC: ", p_arp->ar_sha);
		printf("arp source ip: %s\n", inet_ntoa(p_arp->ar_sip));
		printMac("arp dest MAC: ", p_arp->ar_tha);
		printf("arp dest ip: %s\n", inet_ntoa(p_arp->ar_tip));
		printf("**************************************************\n");
	}

	// 关闭套接字
	close(fd);
	return 0;
}

void printMac(const char* p_head, const unsigned char* p_mac)
{
	printf("%s", p_head);
	for (int i = 0; i < ETH_ALEN - 1; i++)
		printf("%02x-", p_mac[i]);
	printf("%02x\n", p_mac[ETH_ALEN - 1]);
}
