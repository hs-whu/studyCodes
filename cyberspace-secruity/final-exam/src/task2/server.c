//
// Created by hs on 2020/5/22.
//

#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <unistd.h>
#include<sys/socket.h>
#include <netinet/in.h>

#define BUF_SIZE 1024
#define PORT 8080
#define UINT_LEN sizeof(unsigned int)

void reply(int fd, const char *buf, struct sockaddr *from, socklen_t len);

int main()
{
    int fd = socket(PF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        perror("create socket error\n");
        exit(-1);
    }

    struct sockaddr_in bindAddr;
    memset(&bindAddr, 0, sizeof(bindAddr));
    bindAddr.sin_family = PF_INET;
    bindAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bindAddr.sin_port = htons(PORT);
    if(bind(fd, (struct sockaddr*)&bindAddr, sizeof(bindAddr)) < 0)
    {
        perror("bind socket error\n");
        close(fd);
        exit(-1);
    }

    char buf[BUF_SIZE];
    struct sockaddr_in from;
    socklen_t len = sizeof(from);
    int ret;
    while(1)
    {
        ret = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr*)&from, &len);
        if(ret < 0)
        {
            perror("recvfrom error\n");
            exit(-1);
        }

        if(ret == 0)
            continue;

        reply(fd, buf, (struct sockaddr*)&from, len);
    }
}

void reply(int fd, const char *buf, struct sockaddr *from, socklen_t len)
{
    char sendbuf[BUF_SIZE];
    unsigned int length = *((unsigned int*)buf);
    *((unsigned int*)sendbuf) = length;
    for(unsigned int i = 0; i < length; i++)
    {
        (sendbuf + UINT_LEN)[i] = (buf + UINT_LEN)[length-1-i];
    }

    sendto(fd, sendbuf, UINT_LEN + length, 0, from, len);
}
