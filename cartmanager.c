/*
Taylor Seale & Ryan Shea
Operating Systems
April 9, 2013

Project 5 - Synchronization & Deadlocks
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>

int main (int argc, char* argv[]){
	char *str;
	for(str=argv[1]; *str; str++){
		char c=*str;
		printf("%c\n",c);
	}
	pthread_t* trains = malloc(atoi(argv[1])*sizeof(pthread_t)); // create an array of threads for as many trains as given
}
