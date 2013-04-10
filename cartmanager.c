/*
Taylor Seale & Ryan Shea
Operating Systems
April 9, 2013

Project 4 - Synchronization & Deadlocks
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <string.h>

typedef struct{
	char direction;
	int trainID;
}train;

pthread_mutex_t intersection = PTHREAD_MUTEX_INITIALIZER; // mutex for the train intersection
pthread_cond_t northQ=PTHREAD_COND_INITIALIZER,eastQ=PTHREAD_COND_INITIALIZER,southQ=PTHREAD_COND_INITIALIZER,westQ=PTHREAD_COND_INITIALIZER;

void arrive(train c){
    pthread_mutex_lock(&intersection);
    char* direction;
    switch(c.direction){
        case 'n':
            direction="North";
            pthread_cond_wait(&northQ,&intersection);
            break;
        case 'e':
            direction="East";
            pthread_cond_wait(&eastQ,&intersection);
            break;
        case 's':
            direction="South";
            pthread_cond_wait(&southQ,&intersection);
            break;
        case 'w':
            direction="West";
            pthread_cond_wait(&westQ,&intersection);
            break;
        default:
            break;
    }
    printf("CART %d from %s arrives at crossing\n",c.trainID,direction);
    pthread_mutex_unlock(&intersection);    
    
}

void cross(train c){
	sleep(1);
}

void leave(train c){

}

//main function for each thread
void *manage_thread(void *c){
    train* cart = (train*)c; //set the data to a variable so it can be used
    //printf("Main:\tTrain %d\t Direction %c\n",cart->trainID,cart->direction);
    arrive(*cart);
    //cross(*cart);
    //leave(*cart);
}

int main (int argc, char* argv[]){
	pthread_t* trains = malloc((int)strlen(argv[1])*sizeof(pthread_t)); // create an array of threads for as many trains as given	
	train* cart = malloc((int)strlen(argv[1])*sizeof(train));
    char *str; // hold argv[1] to iterate through characters
    int id;
	for(str=argv[1],id=0; *str; str++,id++){
		char c=*str;
		cart[id].direction=c;
		cart[id].trainID=id;
		//printf("Cart Direction: %c\tCart ID: %d\n",cart.direction,cart.trainID);
		pthread_create(&trains[id], NULL, manage_thread, (void *)&cart[id]);
	}

    pthread_cond_signal(&northQ);
    pthread_cond_signal(&eastQ);
    pthread_cond_signal(&southQ);
    pthread_cond_signal(&westQ);

    int i;
    for(i=0;i<id;i++){pthread_join(trains[i], NULL);}
    pthread_cond_destroy(&northQ);
    pthread_cond_destroy(&eastQ);
    pthread_cond_destroy(&southQ);
    pthread_cond_destroy(&westQ);
    pthread_mutex_destroy(&intersection);
    pthread_exit(NULL);
    return 0;
}
