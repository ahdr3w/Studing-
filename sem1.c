#include<stdio.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int stat(const char *file_name, struct stat *buf);

//int readlink(const char *path, char *buf, size_t bufsiz);   

int main(int argc, char* argv[]) {
	char file_name1[100];
	char file_name2[100];
	strcpy(file_name1, argv[1]);
	strcpy(file_name2, argv[2]);
	struct stat *buf1;
	struct stat *buf2 ;
	stat (&file_name1, buf1);
	stat (&file_name2, buf2);
	ino_t fd1 = buf1->st_ino;
	ino_t fd2 = buf2->st_ino;

	char buf[1024];
	char buf0[1024];
	

	
	int condition1 = readlink(file_name1, buf, sizeof(buf) -1);
	int condition2 = readlink(file_name2, buf0, sizeof(buf0) -1);
	
//	if (filename1 == filename2)
//		printf("The files are the same\n");
	if (fd1 == fd2)
		printf("The files are hardlinks of another file\n");
	if ((condition1 == 0) || (condition2 == 0))
		printf("One file is symlink to the other file\n");
	return 0;
}

	
