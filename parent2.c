#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>

int child (int index, int ppid) {
	gid_t rat = fork();
	if ((rat == 0)&&(index != 0)){
		int pid = getpid();
		printf("%d %d\n", ppid, pid);
		--index;
		child (index, pid);
	}
	int i;
	wait(&i);
	exit(0);
	
}

int main(int argc, char*argv[]){
	int index = atoi(argv[1]);
	int ppid = getpid();
	printf("%d\n", ppid);
	child(index,ppid);
}
