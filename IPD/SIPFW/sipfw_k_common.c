//
// Created by hs on 2020/6/9.
//

#ifndef __KERNEL__
#define __KERNEL__
#endif /*__KERNEL__*/

#ifndef MODULE
#define MODULE
#endif /*MODULE*/

#ifndef __DEBUG__
#define __DEBUG__
#endif /*__DEBUG__*/

#include "sipfw_k.h"

static void __flush_rule(struct sipfw_list *list);
static void __list_rule(struct file *file, struct sipfw_list *list);
static char* __inet_ntoa(char *buf, int len, __be32 ip);

int insert_rule(struct sipfw_rule *rule)
{
    int ret = 0;
    struct sipfw_list *t_list;
    struct sipfw_rule *t_rule;

    switch (rule->chain)
    {
        case SIPFW_CHAIN_INPUT:
            t_list = &irules;
            break;
        case SIPFW_CHAIN_OUTPUT:
            t_list = &orules;
            break;
        default:
            ret = -1;
            goto RET;
    }

    t_rule = (struct sipfw_rule *)kmalloc(sizeof(struct sipfw_rule), GFP_KERNEL);
    if(t_rule == NULL)
    {
        ret = -1;
        goto RET;
    }
    memcpy(t_rule, rule, sizeof(struct sipfw_rule));

    t_rule->next = t_list->head;
    t_list->head = t_rule;
    t_list->number++;

    RET:
    return ret;
}

int delete_rule(struct sipfw_rule *rule)
{
    int ret = 0;
    struct sipfw_rule *b_rule;
    struct sipfw_rule *f_rule;
    struct sipfw_list *t_list;

    switch (rule->chain)
    {
        case SIPFW_CHAIN_INPUT:
            t_list = &irules;
            break;
        case SIPFW_CHAIN_OUTPUT:
            t_list = &orules;
            break;
        default:
            ret = -1;
            goto RET;
    }

    b_rule = t_list->head;
    f_rule = b_rule->next;
    if(b_rule->snet == rule->snet && b_rule->smask== rule->smask &&
       b_rule->dnet == rule->dnet && b_rule->dmask == rule->dmask &&
       b_rule->sport == rule->sport && b_rule->dport == rule->dport &&
       b_rule->protocol == rule->protocol)
    {
        t_list->head = f_rule;
        kfree(b_rule);
        t_list->number--;
        goto RET;
    }

    while (f_rule != NULL)
    {
        if(f_rule->snet == rule->snet && f_rule->smask== rule->smask &&
           f_rule->dnet == rule->dnet && f_rule->dmask == rule->dmask &&
           f_rule->sport == rule->sport && f_rule->dport == rule->dport &&
           f_rule->protocol == rule->protocol)
        {
            b_rule->next = f_rule->next;
            kfree(f_rule);
            t_list->number--;
            goto RET;
        }
        b_rule = f_rule;
        f_rule = b_rule->next;
    }
    ret = -1;

    RET:
    return ret;
}

void flush_rule(void)
{
    __flush_rule(&irules);
    __flush_rule(&orules);
}

static void __flush_rule(struct sipfw_list *list)
{
    struct sipfw_rule *t_rule;
    struct sipfw_rule *p_rule;

    t_rule = list->head;
    list->head = NULL;
    list->number = 0;

    while (t_rule != NULL)
    {
        p_rule = t_rule->next;
        kfree_link(t_rule);
        t_rule = p_rule;
    }
}

void list_rule(void)
{
    struct file * file;

    file = SIPFW_OpenFile("/etc/sipfw_rule.conf", O_RDWR | O_TRUNC | O_CREAT, 0666);
    if(file == NULL)
    {
        return;
    }

    __list_rule(file, &irules);
    __list_rule(file, &orules);

    SIPFW_CloseFile(file);
}

static void __list_rule(struct file *file, struct sipfw_list *list)
{
    struct sipfw_rule *t_rule;
    char * chainName;
    char * actionName;
    char buf[256];
    char ip_buf1[32];
    char ip_buf2[32];
    char ip_buf3[32];
    char ip_buf4[32];
    size_t len;

    len = snprintf(buf, sizeof(buf), "%s", "#chain--snet--smask--dnet--dmask--sport--dport--protocol--action#\n");
    len = len <= sizeof(buf) ? len : sizeof(buf);
    SIPFW_WriteLine(file, buf, len);

    t_rule = list->head;
    while (t_rule != NULL)
    {
        switch (t_rule->chain)
        {
            case SIPFW_CHAIN_INPUT:
                chainName = "INPUT";
                break;
            case SIPFW_CHAIN_OUTPUT:
                chainName = "OUTPUT";
                break;
            default:
                chainName = "NULL";
                break;
        }

        switch (t_rule->action)
        {
            case SIPFW_ACTION_ACCEPT:
                actionName = "ACCEPT";
                break;
            case SIPFW_ACTION_DROP:
                actionName = "DROP";
                break;
            default:
                actionName = "NULL";
                break;
        }

        len = snprintf(buf, sizeof(buf), "%s--%s--%s--%s--%s--%d--%d--%d--%s\n", chainName,
                __inet_ntoa(ip_buf1, sizeof(ip_buf1), t_rule->snet),
                __inet_ntoa(ip_buf2, sizeof(ip_buf2), t_rule->smask),
                __inet_ntoa(ip_buf3, sizeof(ip_buf3), t_rule->dnet),
                __inet_ntoa(ip_buf4, sizeof(ip_buf4), t_rule->dmask),
                t_rule->sport, t_rule->dport, t_rule->protocol, actionName);
        len = len <= sizeof(buf) ? len : sizeof(buf);
        SIPFW_WriteLine(file, buf, len);

        t_rule = t_rule->next;
    }

    len = snprintf(buf, sizeof(buf), "%s", "#----------------------end chain----------------------#\n");
    len = len <= sizeof(buf) ? len : sizeof(buf);
    SIPFW_WriteLine(file, buf, len);
}

static char* __inet_ntoa(char *buf, int len, __be32 ip)
{
    snprintf(buf, len, "%u.%u.%u.%u", ip & 0xff, (ip >> 8) & 0xff, (ip >> 16) & 0xff, (ip >> 24) & 0xff);
    return buf;
}

int nf_register_hook(struct nf_hook_ops *reg)
{
    struct net *net, *last;
    int ret;

    rtnl_lock();
    for_each_net(net)
    {
        ret = nf_register_net_hook(net, reg);
        if(ret && ret != -ENOENT)
            goto rollback;
    }
    rtnl_unlock();
    return 0;

    rollback:
    last = net;
    for_each_net(net)
    {
        if(net == last)
            break;
        nf_unregister_net_hook(net, reg);
    }
    rtnl_unlock();
    return ret;
}

void nf_unregister_hook(struct nf_hook_ops *reg)
{
    struct net *net;

    rtnl_lock();
    for_each_net(net)
    {
        nf_unregister_net_hook(net, reg);
    }
    rtnl_unlock();
}

unsigned int nf_match(struct sk_buff *skb, struct sipfw_list *t_list)
{
    struct iphdr * iph;
    struct tcphdr * tcp_h;
    struct udphdr * udp_h;
    struct sipfw_rule * t_rule;

    iph = ip_hdr(skb);
    if(iph == NULL)
    {
        goto RET;
    }

    t_rule = t_list->head;
    while(t_rule != NULL)
    {
        if((t_rule->snet == 0 || (iph->saddr & t_rule->smask) == t_rule->snet) &&
           (t_rule->dnet == 0 || (iph->daddr & t_rule->dmask) == t_rule->dnet))
        {
            if(t_rule->protocol == IPPROTO_IP)
            {
                switch (t_rule->action)
                {
                    case SIPFW_ACTION_DROP:
                        return NF_DROP;
                    default:
                        return NF_ACCEPT;
                }
            }

            if(iph->protocol == t_rule->protocol)
            {
                switch (iph->protocol)
                {
                    case IPPROTO_TCP:
                        tcp_h = tcp_hdr(skb);
                        if((t_rule->sport == 0 || tcp_h->source == t_rule->sport) &&
                           (t_rule->dport == 0 || tcp_h->dest == t_rule->dport))
                        {
                            switch (t_rule->action)
                            {
                                case SIPFW_ACTION_DROP:
                                    return NF_DROP;
                                default:
                                    return NF_ACCEPT;
                            }
                        }
                        break;
                    case IPPROTO_UDP:
                        udp_h = udp_hdr(skb);
                        if((t_rule->sport == 0 || udp_h->source == t_rule->sport) &&
                           (t_rule->dport == 0 || udp_h->dest == t_rule->dport))
                        {
                            switch (t_rule->action)
                            {
                                case SIPFW_ACTION_DROP:
                                    return NF_DROP;
                                default:
                                    return NF_ACCEPT;
                            }
                        }
                        break;
                    default:
                        switch (t_rule->action)
                        {
                            case SIPFW_ACTION_DROP:
                                return NF_DROP;
                            default:
                                return NF_ACCEPT;
                        }
                }
            }
        }

        t_rule = t_rule->next;
    }

    RET:
    return NF_ACCEPT;
}