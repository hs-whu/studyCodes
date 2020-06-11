//
// Created by hs on 2020/6/4.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <linux/types.h>

struct sipfw_rule
{
    int chain;
    __be32 snet;
    __be32 smask;
    __be32 dnet;
    __be32 dmask;
    __be16 sport;
    __be16 dport;
    __u8 protocol;
    int action;
#ifdef __KERNEL__
    struct sipfw_rule *next;
#endif
};

enum
{
    SIPFW_CHAIN_INPUT = 1,
    SIPFW_CHAIN_OUTPUT,

    SIPFW_ACTION_DROP,
    SIPFW_ACTION_ACCEPT,

    SIPFW_CMD_INSERT = 0x6001,
    SIPFW_CMD_DELETE,
    SIPFW_CMD_FLUSH,
    SIPFW_CMD_LIST,
};


int main()
{
    struct sipfw_rule rule;
    socklen_t len = sizeof(rule);

    int fd = socket(PF_INET, SOCK_RAW, IPPROTO_RAW);
    if(fd == -1)
    {
        perror("create socket error\n");
        exit(-1);
    }

    rule.chain = SIPFW_CHAIN_OUTPUT;
    rule.snet = 0;
    rule.smask = 0;
    rule.dnet = 0;
    rule.dmask = 0;
    rule.sport = 0;
    rule.dport = 0;
    rule.protocol = IPPROTO_IP;
    rule.action = SIPFW_ACTION_DROP;

    if(setsockopt(fd, IPPROTO_IP, SIPFW_CMD_INSERT, &rule, len) == -1)
    {
        perror("set sock opt error1\n");
        printf("errno: %d\n", errno);
        exit(-1);
    }

    printf("success1\n");


    if(setsockopt(fd, IPPROTO_IP, SIPFW_CMD_LIST, &rule, len) == -1)
    {
        perror("set sock opt error2\n");
        printf("errno: %d\n", errno);
        exit(-1);
    }

    printf("success2\n");

    return 0;
}