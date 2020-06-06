//
// Created by hs on 2020/4/13.
//

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

void tranFile(char *filepath);
int nameIndex(char *path);

#define serverPort 8089

char * serverIp = "127.0.0.1";

int main()
{
    tranFile("/tmp/hs.txt");
    return 0;
}

void tranFile(char *filepath)
{
    char buf[1024];
    struct stat st;
    unsigned int file_size;

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = PF_INET;
    inet_aton(serverIp, &serverAddr.sin_addr);
    serverAddr.sin_port = htons(serverPort);

    if(stat(filepath, &st) == -1)
        return;
    file_size = st.st_size;

    int read_fd = open(filepath, O_RDONLY);
    if(read_fd == -1)
        return;

    int sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    if(sock_fd == -1)
    {
        close(read_fd);
        return;
    }

    if(connect(sock_fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
    {
        close(read_fd);
        close(sock_fd);
        return;
    }

    snprintf(buf, sizeof(buf), "%d\n%s\n", file_size, filepath + nameIndex(filepath));
    if(write(sock_fd, buf, strlen(buf)) == -1)
    {
        close(read_fd);
        close(sock_fd);
        return;
    }

    size_t size = 0;
    while((size = read(read_fd, buf, sizeof(buf))) != 0)
    {
        if(size == -1)
        {
            close(read_fd);
            close(sock_fd);
            return;
        }

        if(write(sock_fd, buf, size) < size)
        {
            close(read_fd);
            close(sock_fd);
            return;
        }
    }

    close(read_fd);
    close(sock_fd);
}

int nameIndex(char *path)
{
    int len = strlen(path);
    int index = -1;
    for(int i = 0; i < len; i++)
    {
        if(path[i] == '/')
            index = i;
    }
    return index + 1;
}