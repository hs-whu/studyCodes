//
// Created by hs on 2020/2/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <sys/time.h>
#include <errno.h>

#include "ip.h"

struct itimerval val_alarm = { .it_interval.tv_sec = 0, .it_interval.tv_usec = 100000, .it_value.tv_sec = 0, .it_value.tv_usec = 500 };


int main(int argc, char **argv)
{
    struct hostent* dest_host;

    if (argc < 2)
    {
        printf("Usage: %s hostname\n", argv[0]);
        exit(-1);
    }

    if ((dest_host = gethostbyname(argv[1])) == NULL)
    {
        perror("can not understand the host name!\n");
        exit(-1);
    }

    memset(&dest, 0, sizeof(dest));
    dest.sin_family = PF_INET;
    dest.sin_port = htons(0);
    dest.sin_addr = *(struct in_addr*)dest_host->h_addr_list[0];

    memset(&local, 0, sizeof(local));
    local.sin_family = PF_INET;
    local.sin_port = htons(0);
    inet_aton("192.168.100.1", &local.sin_addr);

    if ((sockfd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
    {
        perror("raw socket create error!\n");
        exit(-1);
    }

    int set = 1;
    if(setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &set, sizeof(set)) < 0)
    {
        perror("setsockopt error!\n");
        exit(-1);
    }

    /*if(bind(sockfd, (struct sockaddr*)&local, sizeof(local)) < 0)
    {
        perror("bind error!\n");
        exit(-1);
    }*/

    set_sighandler();

    printf("TraceRoute %s(%s): %d bytes data in ICMP packets.\n\n", argv[1], inet_ntoa(dest.sin_addr), ICMP_DATALEN);

    if ((setitimer(ITIMER_REAL, &val_alarm, NULL)) == -1)
    {
        bail("setitimer fails.\n");
    }

    recv_reply();

    return 0;
}

void send_ping()
{
    struct timeval nowtime;
    gettimeofday(&nowtime, NULL);
    double timespace = (nowtime.tv_sec - lasttime.tv_sec) * 1000.0 + nowtime.tv_usec - lasttime.tv_usec / 1000.0;
    if(timespace > MAXTIME || recv_flag == 1)
    {
        lasttime = nowtime;
        ttl++;
        recv_flag = 0;
    }

    int icmp_len = sizeof(struct icmphdr) + ICMP_DATALEN;
    struct icmphdr* p_icmp;
    p_icmp = (struct icmphdr*)((uint8_t*)sendbuf + sizeof(struct iphdr));
    p_icmp->type = ICMP_ECHO;
    p_icmp->code = 0;
    p_icmp->checksum = 0;
    p_icmp->un.echo.id = htons(getpid());
    p_icmp->un.echo.sequence = htons(icmp_sequence++);
    p_icmp->checksum = checksum((uint8_t*)p_icmp, icmp_len);

    int ip_len = sizeof(struct iphdr) + icmp_len;
    struct iphdr* p_ip;
    p_ip = (struct iphdr*)sendbuf;
    p_ip->version = IPVERSION;
    p_ip->ihl = sizeof(struct iphdr) >> 2;
    p_ip->tos = 0;
    p_ip->tot_len = htons(ip_len);
    p_ip->id = htons(icmp_sequence);
    p_ip->frag_off = htons(0);
    p_ip->ttl = ttl;
    p_ip->protocol = IPPROTO_ICMP;
    p_ip->check = 0;
    p_ip->saddr = local.sin_addr.s_addr;
    p_ip->daddr = dest.sin_addr.s_addr;
    p_ip->check = checksum((uint8_t*)p_ip, ip_len);

    int size = sendto(sockfd, sendbuf, ip_len, 0, (struct sockaddr*)&dest, sizeof(dest));
    if(size != ip_len)
    {
        perror("send error!\n");
        exit(-1);
    }
}

void recv_reply()
{
    while (1)
    {
        socklen_t addrlen;
        int size = recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr*)&from, &addrlen);
        if (size < 0)
        {
            if (errno == EINTR)
                continue;
            bail("recvfrom error");
        }

        if (handler_pkt())
            continue;

        if(size == -1)
            break;
    }
}


int handler_pkt()
{
    struct iphdr* p_ip;
    struct icmphdr* p_icmp;

    int ip_hlen;
    uint16_t ip_datalen;

    p_ip = (struct iphdr*)recvbuf;

    ip_hlen = p_ip->ihl << 2;
    ip_datalen = ntohs(p_ip->tot_len) - ip_hlen;

    p_icmp = (struct icmphdr*)(recvbuf + ip_hlen);

    if (checksum((uint8_t*)p_icmp, ip_datalen))
        return -1;

    if(p_icmp->type != 11)
        return -1;

    printf("router %d: %s\n", ttl, inet_ntoa(from.sin_addr));
    recv_flag = 1;

    return 0;
}

uint16_t checksum(uint8_t * buf, int len)
{
    uint32_t sum = 0;
    uint16_t* cbuf;

    cbuf = (uint16_t*)buf;

    while (len > 1)
    {
        sum += *cbuf++;
        len -= 2;
    }

    if (len)
        sum += *(uint8_t *)cbuf;

    sum = (sum >> 16) + (sum & 0x0ffff);
    sum += (sum >> 16);

    return ~sum;
}

void set_sighandler()
{
    act_alarm.sa_handler = alarm_handler;
    if (sigaction(SIGALRM, &act_alarm, NULL) == -1)
        bail("SIGALRM handler setting fails.\n");

    act_int.sa_handler = int_handler;
    if (sigaction(SIGINT, &act_int, NULL) == -1)
        bail("SIGINT handler setting fails.\n");
}

void bail(const char* on_what)
{
    fputs(strerror(errno), stderr);
    fputs(": ", stderr);
    fputs(on_what, stderr);
    fputc('\n', stderr);
    exit(1);
}

void int_handler(int signo)
{
    close(sockfd);
    exit(1);
}

void alarm_handler(int signo)
{
    send_ping();
}