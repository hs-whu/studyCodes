#pragma pack(1)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/if_ether.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
	int fd;
	if ((fd = socket(PF_PACKET, SOCK_RAW, htons(0x0003))) < 0)
	{
		perror("raw socket create error\n");
		exit(-1);
	}

	char* ethname = "enp2s0";
	struct ifreq ifr;
	strcpy(ifr.ifr_name, ethname);
	int i = ioctl(fd, SIOCGIFFLAGS, &ifr);
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
	}

	char ef[ETH_FRAME_LEN];
	struct ethhdr* p_ethhdr;
	int n;
	p_ethhdr = (struct ethhdr*)ef;
	n = read(fd, ef, ETH_FRAME_LEN);

	printf("dest MAC: ");
	for (int i = 0; i < ETH_ALEN - 1; i++)
		printf("%02x-", p_ethhdr->h_dest[i]);
	printf("%02x\n", p_ethhdr->h_dest[ETH_ALEN - 1]);

	printf("source MAC: ");
	for (int i = 0; i < ETH_ALEN - 1; i++)
		printf("%02x-", p_ethhdr->h_source[i]);
	printf("%02x\n", p_ethhdr->h_source[ETH_ALEN - 1]);

	printf("protocol: 0x%04x\n", ntohs(p_ethhdr->h_proto));

	return 0;
}
