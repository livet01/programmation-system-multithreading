#define _POSIX_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>

#define SON 0

int processus;

void son_handler(int sig){
	if(sig == SIGUSR1){
		printf("Son: --->down<---\n");
		kill(getppid(),SIGCHLD);
		exit(1);
	}
}

void father_handler(int sig){
	if(sig == SIGUSR1){
		printf("Father: --->hit<---\n");
		exit(1);
	}
}

int main(int argc, char ** argv){
	sigset_t mask;
	struct sigaction signal_conf;

	sigfillset(&mask);
	sigdelset(&mask,SIGUSR1);
	signal_conf.sa_flags = 0;

	processus = fork();
	if(processus == SON){
		signal_conf.sa_handler = son_handler;
		signal_conf.sa_mask = mask;

		sigaction(SIGUSR1,&signal_conf,NULL);

		while(1){
			printf("Son: I'm alive\n");
			sleep(10);
		}
	}
	else{
		sigdelset(&mask,SIGCHLD);
		signal_conf.sa_handler = father_handler;
		signal_conf.sa_mask = mask;

		sigaction(SIGUSR1,&signal_conf,NULL);

		sleep(10);		
		printf("Father: I'll kill you son\n");	
		kill(processus,SIGUSR1);
		wait(NULL);
		while(1){
			printf("Father: I'm your father\n");
			sleep(10);
		}
	}
	return 0;
}