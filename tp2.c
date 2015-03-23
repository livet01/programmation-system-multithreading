#define _POSIX_SOURCE
#define _XOPEN_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define KEY 200
int total_sem = 1;

int sem_create(){
	int semaphore;

	semaphore = semget((key_t) KEY, total_sem,0666|IPC_CREAT);
	if(semaphore == -1){
		perror("semget");
		exit(EXIT_FAILURE);
	}
	total_sem ++;

	if(semctl(semaphore,0,SETVAL,1) != 0){
		perror("semctl");
		exit(EXIT_FAILURE);
	}
	return semaphore;
}

void sem_acquire(int semaphore){
	struct sembuf buf;
	buf.sem_num = 0;
	buf.sem_flg = 0;
	buf.sem_op = -1,
	semop(semaphore, &buf, 1);
}

void sem_release(int semaphore){
	struct sembuf buf;
	buf.sem_num = 0;
	buf.sem_flg = 0;
	buf.sem_op = 1,
	semop(semaphore, &buf, 1);
}


int main(int argc, char ** argv){
	int son_processus = 0;
	char buf[256];
	int sem;
	
	sem = sem_create();

	son_processus = fork();
	if(son_processus == -1){
		perror("fork");
		exit(EXIT_FAILURE);
	}
	else if(son_processus == 0){
		
		while(1){
			sem_acquire(sem);
			printf("Son, what ? ");
			gets(buf);
			printf("Son, you said : %s\n",buf);
			sem_release(sem);

			sleep(1);
		}
	}
	else{
		
		while(1){
			sem_acquire(sem);
			printf("Father, what ? ");
			gets(buf);
			printf("Father, you said : %s\n",buf);
			sem_release(sem);

			sleep(1);
		}
	}
	return EXIT_SUCCESS;
}