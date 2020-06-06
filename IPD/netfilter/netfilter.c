//
// Created by hs on 2020/6/3.
//

#include <uapi/linux/netfilter_ipv4.h>
#include <linux/netfilter.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <net/tcp.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

static unsigned int nf_hook_in(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);
static unsigned int nf_hook_out(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);
static int nf_sockopt_set(struct sock *sk, int optval, void __user *user, unsigned int len);
static int nf_sockopt_get(struct sock *sk, int optval, void __user *user, int *len);
static int nf_register_hook(struct nf_hook_ops *reg);
static void nf_unregister_hook(struct nf_hook_ops *reg);

#define SOE_BANDIP 0X6001
#define SOE_BANDPORT 0X6002
#define SOE_BANDPING 0X6003

#define NF_SUCCESS 0
#define NF_FAILURE 1

struct nf_bandport
{
    unsigned short protocol;

    unsigned short port;
};

struct band_status
{
    unsigned int band_ip;

    struct nf_bandport band_port;

    unsigned char band_ping;
};

#define IS_BANDPORT_TCP(status) (status.band_port.protocol == IPPROTO_TCP && status.band_port.port != 0)
#define IS_BANDPORT_UDP(status) (status.band_port.protocol == IPPROTO_UDP && status.band_port.port != 0)
#define IS_BANDPING(status) (status.band_ping)
#define IS_BANDIP(status) (status.band_ip)


struct band_status b_status;
static struct nf_hook_ops nfin = {.hook = nf_hook_in, .hooknum = NF_INET_LOCAL_IN, .pf = PF_INET, .priority = NF_IP_PRI_FILTER};
static struct nf_hook_ops nfout = {.hook = nf_hook_out, .hooknum = NF_INET_LOCAL_OUT, .pf = PF_INET, .priority = NF_IP_PRI_FILTER};
static struct nf_sockopt_ops nfsockopt = {.pf = PF_INET, .set_optmin = SOE_BANDIP, .set_optmax = SOE_BANDIP+3,
        .set = nf_sockopt_set, .get_optmin = SOE_BANDIP, .get_optmax = SOE_BANDIP+3, .get = nf_sockopt_get};


static int nf_sockopt_set(struct sock *sk, int optval, void __user *user, unsigned int len)
{
    int ret = 0;
    struct band_status status;

    if(!capable(CAP_NET_ADMIN))
    {
        ret = -EPERM;
        goto ERROR;
    }

    ret = copy_from_user(&status, user, len);
    if(ret != 0)
    {
        ret = -EINVAL;
        goto ERROR;
    }

    switch (optval)
    {
        case SOE_BANDIP:
            b_status.band_ip = status.band_ip;
            break;
        case SOE_BANDPORT:
            if(IS_BANDPORT_TCP(status) || IS_BANDPORT_UDP(status))
                b_status.band_port = status.band_port;
            else
            {
                b_status.band_port.protocol = 0;
                b_status.band_port.port = 0;
            }
            break;
        case SOE_BANDPING:
            b_status.band_ping = IS_BANDPING(status) ? 1 : 0;
            break;
        default:
            ret = -EINVAL;
            break;
    }

    ERROR:
    return ret;
}

static int nf_sockopt_get(struct sock *sk, int optval, void __user *user, int *len)
{
    int ret = 0;

    if(!capable(CAP_NET_ADMIN))
    {
        ret = -EPERM;
        goto ERROR;
    }

    switch (optval)
    {
        case SOE_BANDIP:
        case SOE_BANDPORT:
        case SOE_BANDPING:
            ret = copy_to_user(user, &b_status, sizeof(b_status));
            if(ret != 0)
            {
                ret = -EINVAL;
                goto ERROR;
            }
            *len = sizeof(b_status);
            break;
        default:
            break;
    }

    ERROR:
    return ret;
}

static unsigned int nf_hook_out(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    struct iphdr *iph = ip_hdr(skb);

    switch (iph->protocol)
    {
        case IPPROTO_ICMP:
            if(IS_BANDPING(b_status))
            {
                printk(KERN_ALERT "drop one icmp packet\n");
                return NF_DROP;
            }
            break;
        default:
            break;
    }

    return NF_ACCEPT;
}

static unsigned int nf_hook_in(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    struct iphdr *iph = ip_hdr(skb);
    struct tcphdr *tcph = NULL;
    struct udphdr *udph = NULL;

    if(IS_BANDIP(b_status))
    {
        if(b_status.band_ip == iph->saddr)
        {
            return NF_DROP;
        }
    }

    switch(iph->protocol)
    {
        case IPPROTO_TCP:
            if(IS_BANDPORT_TCP(b_status))
            {
                tcph = tcp_hdr(skb);
                if(tcph->dest == b_status.band_port.port)
                {
                    return NF_DROP;
                }
            }
            break;
        case IPPROTO_UDP:
            if(IS_BANDPORT_UDP(b_status))
            {
                udph = udp_hdr(skb);
                if(udph->dest == b_status.band_port.port)
                {
                    return NF_DROP;
                }
            }
            break;
        default:
            break;
    }

    return NF_ACCEPT;
}

static int nf_register_hook(struct nf_hook_ops *reg)
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

static void nf_unregister_hook(struct nf_hook_ops *reg)
{
    struct net *net;

    rtnl_lock();
    for_each_net(net)
    {
        nf_unregister_net_hook(net, reg);
    }
    rtnl_unlock();
}

static int __init helloworld_init(void)
{
    int ret = 0;
    ret += nf_register_hook(&nfin);
    ret += nf_register_hook(&nfout);
    ret += nf_register_sockopt(&nfsockopt);

    printk(KERN_ALERT "module init with ret: %d\n", ret);

    return ret ? NF_FAILURE : NF_SUCCESS;
}

static void __exit helloworld_exit(void)
{
    nf_unregister_hook(&nfin);
    nf_unregister_hook(&nfout);
    nf_unregister_sockopt(&nfsockopt);

    printk(KERN_ALERT "module exit");
}

module_init(helloworld_init);
module_exit(helloworld_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("HS");
MODULE_DESCRIPTION("NETFILTER DEMO");
MODULE_VERSION("0.0.1");
MODULE_ALIAS("HELLOWORLD");