#define _POSIX_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>

#define SON 0

int processus;

void handler(int sig){
	if(sig == SIGUSR1){
		if(processus == SON){
			printf("Son: --->down<---\n");
			kill(getppid(),SIGCHLD);
			exit(1);
		}
		else{
			printf("Father: --->hit<---\n");
			exit(1);
		}
	}
}

int main(int argc, char ** argv){

	signal(SIGUSR1,handler);
	processus = fork();
	if(processus == SON){
		while(1){
			printf("Son: I'm alive\n");
			sleep(10);
		}
	}
	else{
		sleep(10);	
		printf("Father: I'll kill you son\n");	
		kill(processus,SIGUSR1);
		while(1){
			printf("Father: I'm your father\n");
			sleep(10);
		}
	}
	return 0;
}