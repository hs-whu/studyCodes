//
// Created by hs on 2020/6/8.
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

/*打开文件*/
struct file *SIPFW_OpenFile(const char *filename, int flags, int mode)
{
    struct file *f = NULL;

    DBGPRINT("==>SIPFW_OpenFile\n");

    f = filp_open(filename, flags, mode);	/*filp_open打开文件*/
    if (!f || IS_ERR(f))				/*判断错误*/
    {
        f = NULL;
    }

    DBGPRINT("<==SIPFW_OpenFile\n");
    return f;
}

/*向文件中写入一行*/
ssize_t SIPFW_WriteLine(struct file *f, char *buf, size_t len)
{
    ssize_t count = -1;
    mm_segment_t oldfs;
    DBGPRINT("==>SIPFW_WriteLine\n");

    /*判断输入参数的正确性*/
    if (!f || IS_ERR(f) || !buf || len <= 0)
    {
        goto out_error;
    }

    /*判断文件指针是否正确*/
    if (!f->f_inode)
    {
        goto out_error;
    }

    /*判断文件权限是否可写*/
    if (!(f->f_mode & FMODE_WRITE) || !(f->f_mode & FMODE_READ) )
    {
        goto out_error;
    }

    oldfs = get_fs();			/*获得地址设置*/
    set_fs(KERNEL_DS);		/*设置为内核模式*/
    count = vfs_write(f, buf, len, &f->f_pos);
    set_fs(oldfs);					/*回复原来的地址设置方式*/

    out_error:
    DBGPRINT("<==SIPFW_WriteLine\n");
    return count;
}

/*关闭文件*/
void SIPFW_CloseFile(struct file *f)
{
    DBGPRINT("==>SIPFW_CloseFile\n");
    if(!f)
        return;

    filp_close(f, current->files);
    DBGPRINT("<==SIPFW_CloseFile\n");
}