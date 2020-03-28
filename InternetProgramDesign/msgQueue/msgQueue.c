//
// Created by hs on 2020/3/10.
// ipcs -q
// ipcrm -qs
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <signal.h>
#include <asm/errno.h>
#include <errno.h>

#define SIGUSER_RECV 20
#define SIGUSER_F_END 21
#define SIGUSER_S_END 22
#define cProcRecvType 12
#define pProcRecvType 32

struct msgbuf
{
    long type;
    char text[100]
};

int msg_id;
pid_t sidePid;
int send_type;
int recv_type;
struct msgbuf send_buf;
struct msgbuf recv_buf;

void createMsgQueue();
void sendMsg();
void closeMsgQueue();
void setSigHandler();
void recv_handler(int sig);
void f_end_handler(int sig);
void s_end_handler(int sig);
void endMsg();

int main()
{
    createMsgQueue();
    setSigHandler();

    pid_t pid;
    if((pid = fork()) == -1)
    {
        perror("fork fail\n");
        closeMsgQueue();
        exit(-1);
    }else if(pid == 0)
    {
        printf("child process\n");
        sidePid = getppid();
        send_type = pProcRecvType;
        recv_type = cProcRecvType;
    } else
    {
        printf("parent process\n");
        sidePid = pid;
        send_type = cProcRecvType;
        recv_type = pProcRecvType;
    }
    sendMsg();

    return 0;
}

void createMsgQueue()
{
    key_t key;

    // create key
    key = ftok(".", 'a');
    if(key == -1)
    {
        perror("建立Key失败\n");
        exit(-1);
    }

    // create msg queue
    msg_id = msgget(key, IPC_CREAT | 0666);
    if(msg_id == -1)
    {
        perror("建立消息队列失败\n");
        exit(-1);
    }
}

void sendMsg()
{
    send_buf.type = send_type;
    while(1)
    {
        // 产生msg
        scanf("%s", send_buf.text);

        // 结束通信
        if(strcmp(send_buf.text, "exit") == 0)
        {
            endMsg();
            continue;
        }

        // 发送msg
        if((msgsnd(msg_id, &send_buf, sizeof(send_buf.text), 0)) == -1)
        {
            perror("发送消息失败\n");
            if(errno == EIDRM)
                exit(1);
            continue;
        }
        kill(sidePid, SIGUSER_RECV);
    }
}

void closeMsgQueue()
{
    if(msgctl(msg_id, IPC_RMID, NULL)) {
        perror("删除消息队列失败\n");
        exit(-1);
    }
}

void setSigHandler()
{
    sigset_t sigset;
    sigemptyset(&sigset);
    sigprocmask(SIG_BLOCK, NULL, &sigset);

    struct sigaction act_recv;
    sigaddset(&sigset, SIGUSER_RECV);
    act_recv.sa_handler = recv_handler;
    act_recv.sa_mask = sigset;
    act_recv.sa_flags = SA_RESTART;
    if(sigaction(SIGUSER_RECV, &act_recv, NULL) == -1)
    {
        perror("Recv handler set fail\n");
        closeMsgQueue();
        exit(-1);
    }

    struct sigaction act_f_end;
    sigaddset(&sigset, SIGUSER_F_END);
    sigaddset(&sigset, SIGUSER_S_END);
    act_f_end.sa_handler = f_end_handler;
    act_f_end.sa_mask = sigset;
    if(sigaction(SIGUSER_F_END, &act_f_end, NULL) == -1)
    {
        perror("F_End handler set fail\n");
        closeMsgQueue();
        exit(-1);
    }

    struct sigaction act_s_end;
    act_s_end.sa_handler = s_end_handler;
    act_s_end.sa_mask = sigset;
    if(sigaction(SIGUSER_S_END, &act_s_end, NULL) == -1)
    {
        perror("Recv handler set fail\n");
        closeMsgQueue();
        exit(-1);
    }
}

void recv_handler(int sig)
{
    while((msgrcv(msg_id, &recv_buf, sizeof(recv_buf.text), recv_type, IPC_NOWAIT | MSG_NOERROR)) != -1)
    {
        if(errno == EIDRM)
        {
            perror("消息队列已经删除\n");
            exit(1);
        }
        printf("接收到msg: %s\n", recv_buf.text);
    }
}

void f_end_handler(int sig)
{
    while((msgrcv(msg_id, &recv_buf, sizeof(recv_buf.text), recv_type, IPC_NOWAIT | MSG_NOERROR)) != -1)
    {
        if(errno == EIDRM)
        {
            perror("消息队列已经删除\n");
            exit(1);
        }
        printf("接收到msg: %s\n", recv_buf.text);
    }
    kill(sidePid, SIGUSER_S_END);
    exit(0);
}

void s_end_handler(int sig)
{
    while((msgrcv(msg_id, &recv_buf, sizeof(recv_buf.text), recv_type, IPC_NOWAIT | MSG_NOERROR)) != -1)
    {
        if(errno == EIDRM)
        {
            perror("消息队列已经删除\n");
            exit(1);
        }
        printf("接收到msg: %s\n", recv_buf.text);
    }
    closeMsgQueue();
    exit(0);
}

void endMsg()
{
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSER_RECV);
    sigaddset(&sigset, SIGUSER_F_END);
    sigprocmask(SIG_BLOCK, &sigset, NULL);
    kill(sidePid, SIGUSER_F_END);
}
