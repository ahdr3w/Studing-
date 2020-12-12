#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<time.h>


struct {
	time_t seconds;
	long nanoseconds;
} ts;
int main(int argc, char*argv[]) {
	int start_time = clock_gettime();
	gid_t rat = fork();
	if (rat == 0) {
		execvp(argv[1], &argv[1]);
		exit(0);
	}
	int i;
	wait(&i);
	int end_time = clock_gettime();
	double delta_time = end_time - start_time;
	printf ("delay: %f", delta_time / CLOCKS_PER_SEC);
}
	
		
	
		
 



