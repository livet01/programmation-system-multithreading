#define _POSIX_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
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

	processus = fork();
	if(processus == SON){
		signal(SIGUSR1,son_handler);
		while(1){
			printf("Son: I'm alive\n");
			sleep(10);
		}
	}
	else{
		signal(SIGUSR1,father_handler);
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