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

int count = 0;
pthread_mutex_t intersection = PTHREAD_MUTEX_INITIALIZER; // mutex for the train intersection
pthread_cond_t northQ,eastQ,southQ,westQ;
//bool ncurr = false, ecurr = false, scurr = false, wcurr = false;


void arrive(train c){
    pthread_mutex_lock(&intersection);
    switch(c.direction){
        case 'n':
            pthread_cond_wait(&northQ,&intersection);
            break;
        case 'e':
            pthread_cond_wait(&eastQ,&intersection);
            break;
        case 's':
            pthread_cond_wait(&southQ,&intersection);
            break;
        case 'w':
            pthread_cond_wait(&westQ,&intersection);
            break;
        default:
            printf("YOU FUCKED UP\n");
            break;
    }
    //count++;
    printf("Arrive:\tTrain %d\t Direction %c\n",c.trainID,c.direction);
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
	char *str; // hold argv[1] to iterate through characters
	int id=1; // overall trainID
	for(str=argv[1]; *str; str++){
		train cart;
		char c=*str;
		cart.direction=c;
		cart.trainID=id;
		//printf("Cart Direction: %c\tCart ID: %d\n",cart.direction,cart.trainID);
		pthread_create(&trains[id-1], NULL, manage_thread, (void *)&cart);
		id++;
	}

    pthread_cond_signal(&northQ);
    pthread_cond_signal(&eastQ);
    pthread_cond_signal(&southQ);
    pthread_cond_signal(&westQ);

    int i;
    for(i=0;i<id;i++){pthread_join(trains[i], NULL);}
    //printf("count %d\n",count);

    pthread_mutex_destroy(&intersection);
    pthread_exit(NULL);
    return 0;
}
