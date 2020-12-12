#include<stdio.h>
#include<sys/shm.h>
#include<sys/wait.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<unistd.h>
#include<errno.h>

//There is condition: left one ship/car process, it as a ship process, the bridge is down
#define One_ship_left ((shared_memory->processes_count - shared_memory->completed_processes == 1) \
		&& (shared_memory->ships_wait == 1) && (shared_memory->bridge_status == CLOSED)) 
#define CLOSED 0
#define OPENED 1

struct {
        short bridge_status;
        int   ships_wait;
	int   processes_count;
	int   completed_processes;
} *shared_memory;

typedef struct {
	int id; // semaphore set identifier
	int num; //every time creating single semaphore 
} semaphore_t;

semaphore_t critical_section;
semaphore_t water_free;
semaphore_t bridge_free;

int  semaphore_determine (semaphore_t sem, int count);
int  sem_lock (semaphore_t sem);
int  sem_open (semaphore_t sem);
void car ();
void ship ();
void ship2 ();
void bridge_choice ();
void initialization();
void work_error (int error_code);
  
int semaphore_determine (semaphore_t sem, int count) {
	struct sembuf sops;
	sops.sem_num = 0;
	sops.sem_op  = count;
       	sops.sem_flg = 0;
	return semop (sem.id, &sops, 1);
}	


int sem_lock(semaphore_t sem) {
	return semaphore_determine (sem, -1);
}

int sem_open(semaphore_t sem) {
	return semaphore_determine (sem, 1);
}

void car() {
	
	sem_lock(bridge_free); //waiting for journey 
	while (1) {
		sem_lock(critical_section); //critical section
			
			if (shared_memory->bridge_status == CLOSED) {  //if the bridge is down 
				printf("Car is starting to move across the bridge\n");
				
				printf("Car passed the bridge\n");
				
				sem_open(bridge_free); // bridge is free
				break;
			}	
		
		sem_open(critical_section);// out of critical section
	}
	sem_open(critical_section);//out of critical section if command BREAK was used
	
}

void ship() {
	
	while (shared_memory->bridge_status == OPENED); /*we wait until the bridge is closed to avoid the situation when 
							  	ships float endlessly under the bridge*/
	sem_lock(critical_section); 
		shared_memory->ships_wait++; // getting in line
		printf("Number of waiting ships: %d\n", shared_memory->ships_wait);
	sem_open(critical_section);
	
	while (shared_memory->bridge_status == CLOSED);

	sem_lock(water_free); // waiting for the turn
	sem_lock(critical_section); //in critical section
		printf("Ship is starting movement\n");
		printf("Ship sailed under the bridge\n");
		sem_open(water_free);
		shared_memory->ships_wait--;// leaving the line
		printf("Number of waiting ships: %d\n", shared_memory->ships_wait);
	sem_open(critical_section);//out of critical section
	
}

void ship2() {
	while (shared_memory->bridge_status == OPENED); /*we wait until the bridge is closed to avoid the\
								 situation when ships float endlessly under the bridge*/
	sem_lock(critical_section); 
		shared_memory->ships_wait++; // getting in line
		printf("Number of waiting ships: %d\n", shared_memory->ships_wait);
	sem_open(critical_section);
	
	sem_lock(water_free); // waiting for the turn 
	while (1) {
		sem_lock(critical_section);//we are in critical section 
		
			if (shared_memory->bridge_status == OPENED) {
				printf("Ship is starting movement\n");
				
				printf("Ship sailed under the bridge\n");
				
				sem_open(water_free);
				shared_memory->ships_wait--;// leaving the line
				printf("Number of waiting ships: %d\n", shared_memory->ships_wait);
				break;
			}
		
		sem_open(critical_section);//out of critical section
	}
	sem_open(critical_section);// out of critical section if you left the loop using BREAK
        
}


void bridge_choice() {
	
	while ((shared_memory->completed_processes < shared_memory->processes_count) && (!(One_ship_left))) {

		sem_lock(critical_section); //we are in critical section 
		
			if ((shared_memory->ships_wait > 1) && (shared_memory->bridge_status == CLOSED)) {
				shared_memory->bridge_status = OPENED;//the bridge is up
				printf("Bridge is up\n");
			}
			if ((shared_memory->ships_wait == 0) && (shared_memory->bridge_status == OPENED)) {
				shared_memory->bridge_status = CLOSED;// the bridge is down
				printf("The bridge is down\n");
			}
		sem_open(critical_section); //out of critical section 
	}

	if (One_ship_left) {
		sem_lock(critical_section); //we are in critical section
			shared_memory->bridge_status = OPENED;//the bridge is up
			printf("One ship is still waiting for pass\n"); 
			printf("Bridge is up\n");
		sem_open(critical_section); //out of critical section 
	}
	exit(EXIT_SUCCESS);
}

void work_error(int error_code) {
	
	switch (error_code) {
		case 1: printf("Failed to create a child process\n");
		case 2: printf("Failed to allocate shared memory\n");
		case 3: printf("Failed to create semaphores\n");
	}
	exit(EXIT_FAILURE);
}

void initialization() {

	int shmid     = shmget(IPC_PRIVATE, sizeof(*shared_memory), 0600); //allocates shared memory
        shared_memory = shmat (shmid, NULL, 0);
        if (shmid == -1)
                work_error(2);

	critical_section.id = semget(IPC_PRIVATE, 1, 0600); // creating new semafores with count = 0
        water_free.id       = semget(IPC_PRIVATE, 1, 0600);
        bridge_free.id      = semget(IPC_PRIVATE, 1, 0600);
        if ((critical_section.id == -1) || (water_free.id == -1) || (bridge_free.id == -1))
                work_error(3);
}

int main (int argc, char* argv[]) {
	
	initialization(); 

	shared_memory->bridge_status       = CLOSED; //as default the bridge is down
	printf("At first bridge is down\n");
        shared_memory->ships_wait          = 0;
	shared_memory->processes_count     = atoi(argv[1]);
	shared_memory->completed_processes = 0;

	sem_open(critical_section);
	sem_open(water_free);
	sem_open(bridge_free);

	short choice   = 0;
	int   wstatus  = 0;
	
	pid_t rat;

	if ((rat = fork()) == 0) {
		bridge_choice();
		exit(EXIT_SUCCESS);
	}
	else
		if (rat == -1) 
			work_error(1);

	srand (getpid()); //choosing generator

	for (int i = 0; i < shared_memory->processes_count; i++) {

		choice = rand() % 2; //what are we going to create car or ship process (gives 0 or N)
		
		if (choice) { 	// on avarage processes will run 1:1
			if ((rat = fork()) == 0) {
	          		car();
				shared_memory->completed_processes++;	
				exit(EXIT_SUCCESS);
			}
			else { 
				if (rat == -1) 
					work_error(1);}
		}
		else {
			if ((rat = fork()) == 0) {	
				ship2();
				shared_memory->completed_processes++;
				exit(EXIT_SUCCESS);
			}
			else {
				if (rat == -1)
					work_error(1);}
		}
	};

	for (int i = 0; i <= shared_memory->processes_count; i++) 
	       wait(&wstatus);
	
	/*cleaning*/
	shmdt (shared_memory);
	semctl(critical_section.id, 0, IPC_RMID); //removing semaphores 
	semctl(water_free.id      , 0, IPC_RMID);
	semctl(bridge_free.id     , 0, IPC_RMID);
	
	printf("+++ exited with 0 +++\n");
	return EXIT_SUCCESS;
}

