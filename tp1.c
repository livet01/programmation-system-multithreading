#define _POSIX_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>

/*
	Son Handler
	Function executed when SIGUSR1 is sended
	Before his death, son send a SIGCHLD to his father (else is not really dead) 
*/
void son_handler(int sig){
	if(sig == SIGUSR1){
		printf("Son: --->down<---\n");
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
	int son_processus = 0;
	sigset_t mask;
	struct sigaction signal_conf;
	
	// SIG conf
	sigfillset(&mask); // Set all bit mask to 1. Default all signals are ignored 
	sigdelset(&mask,SIGUSR1); // Un-mask SIGUSR1
	signal_conf.sa_flags = 0;

	son_processus = fork();
	if(son_processus == 0){
		signal_conf.sa_handler = son_handler;
		signal_conf.sa_mask = mask;

		sigaction(SIGUSR1,&signal_conf,NULL);

		while(1){
			printf("Son: I'm alive\n");
			sleep(10);
		}
	}
	else{
		sigdelset(&mask,SIGCHLD); // Un-mask SIGCHLD to receive son dead ack
		signal_conf.sa_handler = father_handler;
		signal_conf.sa_mask = mask;

		sigaction(SIGUSR1,&signal_conf,NULL);

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