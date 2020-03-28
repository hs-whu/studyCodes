#pragma once

struct arppacket
{
	struct arphdr ar_head;
	unsigned char ar_sha[ETH_ALEN];
	struct in_addr ar_sip;
	unsigned char ar_tha[ETH_ALEN];
	struct in_addr ar_tip;
};