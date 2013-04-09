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
#include <string.h>

typedef struct{
	char direction;
	int qPos;
	int trainID;
}train;

// function to run for each thread
void *arrive(void *c){
	train* cart = (train*)c; // set the data to a variable so it can be used
}

int main (int argc, char* argv[]){
	pthread_t* trains = malloc((int)strlen(argv[1])*sizeof(pthread_t)); // create an array of threads for as many trains as given
	
	char *str; // hold argv[1] to iterate through characters
	int id=0; // overall trainID
	int ncount=0, ecount=0, scount=0, wcount=0; // count for each direction
	for(str=argv[1]; *str; str++){
		train cart;
		char c=*str;
		switch (c){
			case 'n':
				cart.qPos=ncount;
				ncount++;
				break;
			case 'e':
				cart.qPos=ecount;
				ecount++;
				break;
			case 's':
				cart.qPos=scount;
				scount++;
				break;
			case 'w':
				cart.qPos=wcount;
				wcount++;
				break;
		}
		cart.direction=c;
		cart.trainID=id;
		printf("Cart Direction: %c\tCart Queue Pos: %d\tCart ID: %d\n",cart.direction,cart.qPos,cart.trainID);
		pthread_create(&trains[id], NULL, arrive, (void *)&cart);
		id++;
	}

}
