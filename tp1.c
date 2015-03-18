#define _POSIX_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#define PINGPONG_MAX 1000

/*
	Son Handler
	Function executed when SIGUSR1 is sended
	Before his death, son send a SIGCHLD to his father (else is not really dead) 
*/
void son_handler(int sig){
	if(sig == SIGUSR1){
		printf("Son   : --->down<---\n");
		kill(getppid(),SIGCHLD);
		exit(EXIT_SUCCESS);
	}
}

/*
	Father Handler
	Function executed when SIGUSR1 is sended
*/
void father_handler(int sig){
	if(sig == SIGUSR1){
		printf("Father: --->hit<---\n");
		exit(EXIT_SUCCESS);
	}
}

int main(int argc, char ** argv){
	int data = 0, son_processus = 0;
	sigset_t mask;
	struct sigaction signal_conf;
	int father_to_son_pipefd[2], son_to_father_pipefd[2];
	
	// SIG conf
	sigfillset(&mask); // Set all bit mask to 1. Default all signals are ignored 
	sigdelset(&mask,SIGUSR1); // Un-mask SIGUSR1
	signal_conf.sa_flags = 0;

	// PIPE conf
	if (pipe(father_to_son_pipefd) == -1) {
       perror("father_to_son_pipefd");
       exit(EXIT_FAILURE);
   	}
	if (pipe(son_to_father_pipefd) == -1) {
       perror("son_to_father_pipefd");
       exit(EXIT_FAILURE);
   	}

	son_processus = fork();
	if(son_processus == 0){
		close(father_to_son_pipefd[1]); // Close unused write end
		close(son_to_father_pipefd[0]); // Close unused read end

		signal_conf.sa_handler = son_handler;
		signal_conf.sa_mask = mask;

		sigaction(SIGUSR1,&signal_conf,NULL);

		// Read first
		while(data < PINGPONG_MAX){
			read(father_to_son_pipefd[0], &data,sizeof(int));

			data++;
			write(son_to_father_pipefd[1],&data,sizeof(int));
		}	

		close(father_to_son_pipefd[0]);
		close(son_to_father_pipefd[1]);
		while(1){
			printf("Son   : I'm alive\n");
			sleep(10);
		}
	}
	else{
		close(father_to_son_pipefd[0]); // Close unused read end
		close(son_to_father_pipefd[1]); // Close unused write end

		sigdelset(&mask,SIGCHLD); // Un-mask SIGCHLD to receive son dead ack
		signal_conf.sa_handler = father_handler;
		signal_conf.sa_mask = mask;

		sigaction(SIGUSR1,&signal_conf,NULL);

		// Write first
		while(data < PINGPONG_MAX){
			data++;
			write(father_to_son_pipefd[1],&data,sizeof(int));

			read(son_to_father_pipefd[0], &data,sizeof(int));
		}

		close(father_to_son_pipefd[0]);
		close(son_to_father_pipefd[1]);

		sleep(10);		
		printf("Father: I'll kill you son\n");	

		// Send SIGUSR1 to son process
		kill(son_processus,SIGUSR1);
		wait(NULL); // Wait SIGCHLD
		
		while(1){
			printf("Father: I'm your father\n");
			sleep(10);
		}
	}

	return EXIT_SUCCESS;
}