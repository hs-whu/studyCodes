//
// Created by hs on 2020/5/22.
//

#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <unistd.h>
#include<sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024
#define IP_ADDR "127.0.0.1"
#define PORT 8080
#define UINT_LEN sizeof(unsigned int)

char *sendstr = "abcdefghigklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

int main()
{
    int fd = socket(PF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        perror("create socket error\n");
        exit(-1);
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = PF_INET;
    inet_aton(IP_ADDR, &serverAddr.sin_addr);
    serverAddr.sin_port = htons(PORT);

    char buf[BUF_SIZE];
    struct sockaddr_in from;
    socklen_t len = sizeof(from);

    unsigned int size = strlen(sendstr);
    *((unsigned int*)(buf)) = size;
    memcpy(buf+ UINT_LEN, sendstr, size);

    sendto(fd, buf, UINT_LEN + size, 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    printf("send: %s\n", sendstr);
    recvfrom(fd, buf, sizeof(buf),  0, (struct sockaddr*)&from, &len);
    size = *((unsigned int*)buf);
    buf[UINT_LEN + size] = '\0';
    printf("recv: %s\n", buf + UINT_LEN);

    return 0;
}
