//
// Created by hs on 2020/5/25.
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
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/aes.h>

void set_signal();
void init();
void sig_child(int sig);
void readFile(int client_fd);

#define UINTLEN sizeof(unsigned int)
#define serverPort 8089
RSA *p_rsa_private_key;

char * rsakey = "-----BEGIN RSA PRIVATE KEY-----\n"
                "MIICXAIBAAKBgQC27N5fK/kbHfnveRMwXyHJooVCBsVWZdK73VXqVJku47pIK77j\n"
                "WPQLpeH8UWUaLLyXBGsRWx8e2NZICCoEugNpVE1jruat4h7V2la/w94n/Z39Yp2i\n"
                "7z61gLxp4QpMiq8M1rtb9kQZv6QvEUSgFUVxh5VCO3VoI8geeU5XStD/twIDAQAB\n"
                "AoGAOPTsEIoqmEzvI6d5WBhm9teJ0pM1Ir+1rBUwyTPqNnVl7U7hsvxhkLbn9J6L\n"
                "cmj3l7YieFb9C6fMoMUaADrDEKO8Tb0uYiJ3/FwwQ4CFPED6bO1YP0vB5jzNax06\n"
                "2OyX3i9drLzq/wXuNXXyZY4KA5xq7tWSVU4LbliMRPic2WECQQDoOtE3J/Ue9DlY\n"
                "qftBGBSrKudpfwV0qTa1o3foz13mp5luhSAET//s46Y5mUDStqkM8al/jF035tuk\n"
                "i5sjYbe5AkEAyaYcn5ZicJdxuYtAGPt1bIKePk4NtZULMR7RRLT/DpqyuG2u5zFl\n"
                "6V5P71Tqe4tGWn24F00LBFvzEXKARtNK7wJAJhtQrV5PKK8modfytLHA4n19z5/a\n"
                "Q1Ro99FFIdy1Kd4inTIXGN4Pvs10P0tYibsTbl5RU+6ydTPaotuNr3afcQJAA46O\n"
                "irIYYmwJcYBnTQmCdLuJFwhBbaaHYAJvJosaxKMt69rjbuiMJ6WmO06AJFW8k/QL\n"
                "vz14qEcG7pPad2VauQJBAI8UUEcjrSHaQUupdPkktz/pwHKWkGQmz+y6VVIYTvYF\n"
                "IH/nGTs8Lm85e6Z5uZ82tQN3vWAjGQYgwOokzxwnzDU=\n"
                "-----END RSA PRIVATE KEY-----";

int main()
{
    init();
    chdir("/root/txt/");

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
    AES_KEY aes_key;
    unsigned char temp_buf[1024];
    unsigned char buf[1024];
    unsigned int file_size;

    size_t size = read(client_fd, temp_buf,128);
    if(size != 128)
    {
        close(client_fd);
        return;
    }
    RSA_private_decrypt(128, temp_buf, buf, p_rsa_private_key, RSA_NO_PADDING);

    AES_set_decrypt_key(buf, AES_BLOCK_SIZE * 8, &aes_key);
    memset(buf, 0, AES_BLOCK_SIZE);
    file_size = htonl(*((unsigned int*)(buf + AES_BLOCK_SIZE)));

    int write_fd = open((char*)buf + AES_BLOCK_SIZE + UINTLEN, O_WRONLY | O_TRUNC | O_CREAT, 0666);
    if(write_fd == -1)
    {
        close(client_fd);
        return;
    }

    int recvSum = 0;
    int temp = 0;
    while(recvSum < file_size)
    {
        size = read(client_fd, temp_buf + temp, sizeof(temp_buf) - temp);
        if(size == -1)
            break;

        size += temp;
        temp = 0;

        if(size == 0)
            break;

        if(size % AES_BLOCK_SIZE != 0)
        {
            temp = size % AES_BLOCK_SIZE;
            size -= temp;
        }

        for(int i = 0; i < size / AES_BLOCK_SIZE; i++)
            AES_decrypt(temp_buf + i * AES_BLOCK_SIZE, buf + i * AES_BLOCK_SIZE, &aes_key);

        if(recvSum + size > file_size)
            size = file_size - recvSum;

        write(write_fd, buf, size);
        recvSum += size;

        memcpy(temp_buf, temp_buf + size, temp);
    }

    close(client_fd);
    close(write_fd);
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

    BIO * p_bio = BIO_new(BIO_s_mem());
    if(p_bio == NULL)
        exit(-1);
    BIO_puts(p_bio, rsakey);

    p_rsa_private_key = PEM_read_bio_RSAPrivateKey(p_bio, NULL, NULL, NULL);
    if(p_rsa_private_key == NULL)
        exit(-1);
}