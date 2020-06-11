//
// Created by hs on 2020/6/9.
//

#ifndef SIPFW_K_H
#define SIPFW_K_H

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <uapi/linux/netfilter_ipv4.h>
#include <linux/netfilter.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <net/tcp.h>
#include <linux/types.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>

#ifdef __DEBUG__
#define DBGPRINT printk
#else
#define DBGPRINT
#endif

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

struct sipfw_list
{
    struct sipfw_rule *head;
    int number;
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

extern struct sipfw_list irules;
extern struct sipfw_list orules;

extern int insert_rule(struct sipfw_rule *rule);
extern int delete_rule(struct sipfw_rule *rule);
extern void flush_rule(void);
extern void list_rule(void);
extern int nf_register_hook(struct nf_hook_ops *reg);
extern void nf_unregister_hook(struct nf_hook_ops *reg);
extern unsigned int nf_match(struct sk_buff *skb, struct sipfw_list *t_list);

extern struct file *SIPFW_OpenFile(const char *filename, int flags, int mode);
extern ssize_t SIPFW_WriteLine(struct file *f, char *buf, size_t len);
extern void SIPFW_CloseFile(struct file *f);

#endif //SIPFW_K_H
