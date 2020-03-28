#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

char buf[1024];

int main(int argc, char ** argv)
{
	if(argc < 3)
	{
		perror("argument is not enough!\n");
		exit(-1);
	}

	char * read_path = argv[1];
	char * write_path = argv[2];
	
	int read_fd, write_fd;
	
	read_fd = open(read_path, O_RDONLY);
	if(read_fd == -1)
	{
		perror("create read fd error!\n");
		exit(-1);
	}
	
	write_fd = open(write_path, O_WRONLY | O_CREAT | O_TRUNC);
	if(write_fd == -1)
	{
		perror("create write fd error!\n");
		exit(-1);
	}
	
	ssize_t read_size, write_size;
	while(1)
	{
		read_size = read(read_fd, buf, sizeof(buf));
		
		if(read_size == -1)
		{
			perror("read error occured!\n");
			exit(-1);
		}
		
		if(read_size == 0)
		{
			printf("read ends!\n");
			break;
		}
		
		write_size = write(write_fd, buf, read_size);
		
		if(write_size == -1)
		{
			perror("write error occured!\n");
			exit(-1);
		}
		
		if(write_size != read_size)
		{
			perror("write size not euqals to read size!\n");
			exit(-1);
		}
	}
	
	close(read_fd);
	close(write_fd);
	
	return 0;
}
