#define _POSIX_SOURCE
#define _XOPEN_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define KEY 300
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
	char buf[256];
	
	sem_create(2);
	sem_acquire(1);

	son_processus = fork();
	if(son_processus == -1){
		perror("fork");
		exit(EXIT_FAILURE);
	}
	else if(son_processus == 0){
		
		while(1){
			sem_acquire(1);
			printf("Son, what ? ");
			gets(buf);
			printf("Son, you said : %s\n",buf);
			sem_release(0);
		}
	}
	else{
		
		while(1){
			sem_acquire(0);
			printf("Father, what ? ");
			gets(buf);
			printf("Father, you said : %s\n",buf);
			sem_release(1);
		}
	}
	return EXIT_SUCCESS;
}