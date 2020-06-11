//
// Created by hs on 2020/6/3.
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

struct sipfw_list irules;
struct sipfw_list orules;

#define NF_SUCCESS 0
#define NF_FAILURE 1

static unsigned int nf_hook_in(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);
static unsigned int nf_hook_out(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);
static int nf_sockopt_set(struct sock *sk, int optval, void __user *user, unsigned int len);

static struct nf_hook_ops nfin = {.hook = nf_hook_in, .hooknum = NF_INET_LOCAL_IN, .pf = PF_INET, .priority = NF_IP_PRI_FILTER};
static struct nf_hook_ops nfout = {.hook = nf_hook_out, .hooknum = NF_INET_LOCAL_OUT, .pf = PF_INET, .priority = NF_IP_PRI_FILTER};
static struct nf_sockopt_ops nfsockopt = {.pf = PF_INET, .set_optmin = SIPFW_CMD_INSERT, .set_optmax = SIPFW_CMD_INSERT+4,
        .set = nf_sockopt_set, .get_optmin = 0, .get_optmax = 0, .get = NULL};


static int nf_sockopt_set(struct sock *sk, int optval, void __user *user, unsigned int len)
{
    int ret = 0;
    struct sipfw_rule rule;

    if(!capable(CAP_NET_ADMIN))
    {
        ret = -EPERM;
        goto ERROR;
    }

    len = len <= sizeof(struct sipfw_rule) ? len : sizeof(struct sipfw_rule);

    ret = (int)copy_from_user(&rule, user, len);
    if(ret != 0)
    {
        ret = -EINVAL;
        goto ERROR;
    }

    switch(optval)
    {
        case SIPFW_CMD_INSERT:
            ret = insert_rule(&rule);
            if(ret != 0)
            {
                ret = -EPERM;
                goto ERROR;
            }
            break;
        case SIPFW_CMD_DELETE:
            ret = delete_rule(&rule);
            if(ret != 0)
            {
                ret = -EPERM;
                goto ERROR;
            }
            break;
        case SIPFW_CMD_FLUSH:
            flush_rule();
            break;
        case SIPFW_CMD_LIST:
            list_rule();
            break;
        default:
            ret = -EPERM;
            break;
    }

    ERROR:
    return ret;
}

static unsigned int nf_hook_out(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    return nf_match(skb, &orules);
}

static unsigned int nf_hook_in(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    return nf_match(skb, &irules);
}

static int __init sipfw_init(void)
{
    int ret = 0;

    memset(&irules, 0, sizeof(irules));
    memset(&orules, 0, sizeof(orules));

    ret += nf_register_hook(&nfin);
    ret += nf_register_hook(&nfout);
    ret += nf_register_sockopt(&nfsockopt);

    printk(KERN_ALERT "module init with ret: %d\n", ret);

    return ret ? NF_FAILURE : NF_SUCCESS;
}

static void __exit sipfw_exit(void)
{
    nf_unregister_hook(&nfin);
    nf_unregister_hook(&nfout);
    nf_unregister_sockopt(&nfsockopt);

    flush_rule();

    printk(KERN_ALERT "module exit");
}

module_init(sipfw_init);
module_exit(sipfw_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("HS");
MODULE_DESCRIPTION("NETFILTER DEMO");
MODULE_VERSION("0.0.1");
MODULE_ALIAS("HELLOWORLD");
