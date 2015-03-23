#define _POSIX_SOURCE
#define _XOPEN_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>    
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define KEY 300
#define PINGPONG_MAX 2000

int semaphore;

void sem_create(int nb_sem){
	int i;
	semaphore = semget((key_t) KEY, nb_sem,0666|IPC_CREAT);
	if(semaphore == -1){
		perror("semget");
		exit(EXIT_FAILURE);
	}
	for(i=0;i<nb_sem;i++){
		if(semctl(semaphore,i,SETVAL,1) != 0){
			perror("semctl");
			exit(EXIT_FAILURE);
		}
	}
}

void sem_acquire(int sem_num){
	struct sembuf buf;
	buf.sem_num = sem_num;
	buf.sem_flg = 0;
	buf.sem_op = -1;
	if(semop(semaphore, &buf, 1) != 0){
		perror("semop acquire");
		exit(EXIT_FAILURE);
	}
}

void sem_release(int sem_num){
	struct sembuf buf;
	buf.sem_num = sem_num;
	buf.sem_flg = 0;
	buf.sem_op = 1;
	if(semop(semaphore, &buf, 1) != 0){
		perror("semop release");
		exit(EXIT_FAILURE);
	}
}


int main(int argc, char ** argv){
	int son_processus = 0;
	int shm;

	sem_create(2);
	
	shm = shmget((key_t) KEY, sizeof(int), 0666|IPC_CREAT);

	if(shm == -1){
		perror("shmget");
		exit(EXIT_FAILURE);
	}

	son_processus = fork();
	if(son_processus == -1){
		perror("fork");
		exit(EXIT_FAILURE);
	}
	else if(son_processus == 0){
		int * data;
		data = (int *) shmat(shm,0,0);
		if(data==NULL) {
			perror("shmat");
			exit(EXIT_FAILURE);
		}
		do{
			printf("Wait 1");
			sem_acquire(1);
			(*data)++;
			printf("Data : %d\n",*data);
			sem_release(0);
		}while(*data < PINGPONG_MAX);
		
		// release shm
		shmdt(data);
	}
	else{
		int * data;
		struct timeval before, after, delta;
		double debit = 0.0;

		printf("FATHER\n");
		data = (int *) shmat(shm,0,0);
		if(data==NULL) {
			perror("shmat");
			exit(EXIT_FAILURE);
		}

		gettimeofday(&before, NULL);
		*data = 0;
		printf("Data : %d\n",*data);
		sem_release(1);

		while(*data < PINGPONG_MAX){
			sem_acquire(0);
			(*data)++;
			sem_release(1);
		}
		gettimeofday(&after, NULL);
		
		// release shm
		shmdt(data);

		// Debit mesure 
		delta.tv_sec = after.tv_sec - before.tv_sec;
		delta.tv_usec = after.tv_usec - before.tv_usec;
		debit = (1000.0*2.0*4.0*8.0)/delta.tv_usec;
		printf("execution time %ld.%06ld debit %f Mb\n",delta.tv_sec,delta.tv_usec,debit);
	}

	semctl(semaphore,0,IPC_RMID,0);

	return EXIT_SUCCESS;
}