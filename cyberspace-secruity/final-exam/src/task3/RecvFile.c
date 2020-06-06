//
// Created by hs on 2020/5/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <wait.h>

void set_signal();
void init();
void sig_child(int sig);
void readFile(int client_fd);
int line_index(const char *buf, int offset, int len);

#define serverPort 8089

int main()
{
    init();

    struct sockaddr_in bindAddr;
    memset(&bindAddr, 0, sizeof(bindAddr));
    bindAddr.sin_family = PF_INET;
    bindAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bindAddr.sin_port = htons(serverPort);

    int sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    if(sock_fd == -1)
    {
        perror("create socket error\n");
        exit(-1);
    }

    if(bind(sock_fd, (struct sockaddr*)&bindAddr, sizeof(bindAddr)) == -1)
    {
        close(sock_fd);
        perror("bind error\n");
        exit(-1);
    }

    if(listen(sock_fd, 10) == -1)
    {
        close(sock_fd);
        perror("listen error\n");
        exit(-1);
    }

    struct sockaddr_in clientAddr;
    socklen_t socklen = sizeof(clientAddr);
    while(1)
    {
        int client_fd = accept(sock_fd, (struct sockaddr*)&clientAddr, &socklen);
        if(client_fd == -1)
            continue;

        pid_t pid = fork();
        if(pid < 0)
        {
            close(client_fd);
            close(sock_fd);
            exit(-1);
        }

        if(pid > 0)
        {
            close(client_fd);
            continue;
        }

        close(sock_fd);
        readFile(client_fd);
        break;
    }
}

void readFile(int client_fd)
{
    char buf[1024];

    size_t size = read(client_fd, buf, sizeof(buf));
    if(size == -1)
    {
        close(client_fd);
        return;
    }

    int offset1 = line_index(buf, 0, size);
    if(offset1 == -1)
    {
        close(client_fd);
        return;
    }
    buf[offset1] = '\0';
    int file_size = strtol(buf, NULL, 0);

    int offset2 = line_index(buf, offset1 + 1, size);
    if(offset2 == -1)
    {
        close(client_fd);
        return;
    }
    buf[offset2] = '\0';
    int write_fd = open(buf + offset1 + 1, O_WRONLY | O_TRUNC | O_CREAT, 0666);
    if(write_fd == -1)
    {
        close(client_fd);
        return;
    }

    int recvSum = size - offset2 - 1;
    write(write_fd, buf + offset2 + 1, recvSum);
    while(recvSum < file_size)
    {
        size = read(client_fd, buf, sizeof(buf));
        if(size == -1)
            break;

        if(size == 0)
            break;

        write(write_fd, buf, size);
        recvSum += size;
    }

    close(client_fd);
    close(write_fd);
}

void init()
{
    set_signal();

    char *home_dir = getenv("HOME");
    char txt_dir[1024];
    snprintf(txt_dir, sizeof(txt_dir), "%s/txt", home_dir);
    int ret = access(txt_dir, F_OK);
    if(ret == -1)
        mkdir(txt_dir, 0777);

    chdir(txt_dir);
}

void set_signal()
{
    struct sigaction act_child;
    memset(&act_child, 0, sizeof(act_child));
    act_child.sa_handler = sig_child;
    act_child.sa_flags |= SA_RESTART;
    if(sigaction(SIGCHLD, &act_child, NULL) == -1)
    {
        perror("CHLD handler set error\n");
        exit(-1);
    }
}

void sig_child(int sig)
{
    int stat;
    while(waitpid(-1, &stat, WNOHANG) > 0);
}

int line_index(const char *buf, int offset, int len)
{
    for(int i = offset; i < len; i++)
    {
        if(buf[i] == '\n')
            return i;
    }

    return -1;
}