#define _POSIX_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>       
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


#define PINGPONG_MAX 2000
#define FIFO_ABSOLUTE_NAME "/tmp/fifo"

int main(int argc, char ** argv){
	int data = 0;
	int fifo_send, fifo_receive;
	char name_fifo1[20], name_fifo2[20];
	
	if(argc != 3){
		fprintf(stderr,"usage: ./tp1 local_process remote_process\n");
		exit(3);
	}
	strcpy(name_fifo1,FIFO_ABSOLUTE_NAME);
	strcat(name_fifo1,argv[1]);
	strcpy(name_fifo2,FIFO_ABSOLUTE_NAME);
	strcat(name_fifo2,argv[2]);

	if(mkfifo(name_fifo1,S_IRUSR | S_IWUSR | S_IRGRP) != 0){
		perror("mkfifo");
		exit(EXIT_FAILURE);
	}
	
	if(argv[1] > argv[2]){
		fifo_receive = open(name_fifo1,O_RDONLY);
		printf("%s created\n", name_fifo1);
		fifo_send = open(name_fifo2,O_WRONLY);
		printf("%s created\n", name_fifo2);

		// Read first
		while(data < PINGPONG_MAX){
			read(fifo_receive, &data,sizeof(int));

			data++;
			write(fifo_send,&data,sizeof(int));
		}	

		close(fifo_send);
		close(fifo_receive);
		unlink(name_fifo1);
	}
	else {
		struct timeval before, after, delta;
		double debit = 0.0;

		fifo_send = open(name_fifo1,O_RDONLY);
		printf("%s created\n", name_fifo1);
		fifo_receive = open(name_fifo2,O_WRONLY);
		printf("%s created\n", name_fifo2);

		// Write first
		gettimeofday(&before, NULL);
		while(data < PINGPONG_MAX){
			data++;
			write(fifo_send,&data,sizeof(int));

			read(fifo_receive, &data,sizeof(int));
		}
		gettimeofday(&after, NULL);

		// Debit mesure 
		delta.tv_sec = after.tv_sec - before.tv_sec;
		delta.tv_usec = after.tv_usec - before.tv_usec;
		debit = (1000.0*2.0*4.0*8.0)/delta.tv_usec;
		printf("execution time %ld.%06ld debit %f Mb\n",delta.tv_sec,delta.tv_usec,debit);

		close(fifo_send);
		close(fifo_receive);
		unlink(name_fifo1);
	}

	return EXIT_SUCCESS;
}