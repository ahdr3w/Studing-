#include<stdio.h>
#include<pthread.h>
#include<time.h>
#include<stdlib.h>

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;



void forc1 (int b) {
	int local_count = 0;
	for (local_count; local_count <= b; local_count++);
	pthread_mutex_lock(&m);
	count += local_count;
	pthread_mutex_unlock(&m);
	
}

void forc2 (int b) {
	pthread_mutex_lock(&m);
	int i = 0;
	for (i; i <= b; i++);
	count += i;
	pthread_mutex_unlock(&m);
}

int main(int argc, char* argv[]) {
	int count = 0;	
	int N = atoi(argv[1]);
	int a = atoi(argv[2]);
	int b = N/a;
	
	pthread_t thread[a];
	
	
	for (int i = 0; i < a; i++)
	       pthread_create (&thread[i], 0, forc1, &b);
	printf ("%d", count);

	
}
	
	       
	 
