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
	int trainID;
}train;

pthread_mutex_t intersection; // mutex for the train intersection

//main function for each thread
void *thread_main(void *c){
    train* cart = (train*)c; //set the data to a variable so it can be used
    printf("Cart ID: %d\t Cart Direction: %c\n",cart->trainID,cart->direction);
}

void arrive(train c){

}

void cross(train c){

}

void leave(train c){
	
}

int main (int argc, char* argv[]){
	pthread_t* trains = malloc((int)strlen(argv[1])*sizeof(pthread_t)); // create an array of threads for as many trains as given	
	char *str; // hold argv[1] to iterate through characters
	int id=0; // overall trainID
	pthread_mutex_init(&intersection,NULL);
	for(str=argv[1]; *str; str++){
		train cart;
		char c=*str;
		cart.direction=c;
		cart.trainID=id;
		//printf("Cart Direction: %c\tCart ID: %d\n",cart.direction,cart.trainID);
		pthread_create(&trains[id], NULL, thread_main, (void *)&cart);
		id++;
	}

    int i;
    for(i=0;i<id;i++){pthread_join(trains[i], NULL);}
    pthread_mutex_destroy(&intersection);
    pthread_exit(NULL);
    return 0;
}
