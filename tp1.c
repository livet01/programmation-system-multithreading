#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#define FATHER 1
#define SON 0

int processus;

void handler(int sig){
	if(sig == SIGUSR1){
		if(processus == FATHER){
			printf("Father: hit\n");
		}
		else{
			printf("Son: down\n");
		}
	}
}

int main(int argc, char ** argv){

	signal(SIGUSR1,handler);
	processus = fork() == 0;
	if(processus == SON){
		while(1){
		printf("Son: I'm alive\n");
		sleep(10);
		}
	}
	else{
		printf("Father\n");
		sleep(10);		
		while(1);
	}
	return 0;
}