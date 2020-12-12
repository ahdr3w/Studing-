#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#define bits_in_byte 8

int main( int argc, char**argv) {
	const char* file_name = *++argv;
	char buf;
	int fd = open(file_name, O_RDONLY);
	size_t letter = sizeof(char)*bits_in_byte;
	while (&buf != 0){
			read(fd, &buf, letter);
			write(1, &buf, letter);
	}
	close (fd);
}


























