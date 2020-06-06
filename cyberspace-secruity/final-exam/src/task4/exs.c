//
// Created by hs on 2020/5/24.
//

//objdump -d -F shellcode
//提取shellcode十六进制码: exs shellcode 0x60 0x101

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

char buf[1024 * 1024];

int main(int argc, char **argv)
{
    if(argc != 4)
    {
        perror("LIKE: exs filename begOffset len(exs shellcode 0x60 120)");
        exit(-1);
    }

    int fd = open(argv[1], O_RDONLY);
    int begOffset = strtol(argv[2], NULL, 0);
    int len = strtol(argv[3], NULL, 0);

    lseek(fd, begOffset, SEEK_SET);
    read(fd, buf, len);
    for(int i = 0; i < len; i++)
    {
        printf("\\x%02x", (unsigned char)buf[i]);
    }

    return 0;
}