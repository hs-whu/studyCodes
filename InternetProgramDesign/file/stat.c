#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char ** argv)
{
	if(argc < 2)
	{
		perror("argument is not enough!\n");
		exit(-1);
	}

	char * file_path = argv[1];
	struct stat st;
	if(stat(file_path, &st) == -1)
	{
		perror("get file status error!\n");
		exit(-1);
	}
	
	printf("包含此文件的设备ID: %ld\n", st.st_dev);
	printf("此文件的节点: %ld\n", st.st_ino);
	printf("此文件的保护模式: %d\n", st.st_mode);
	
	return 0;
}
