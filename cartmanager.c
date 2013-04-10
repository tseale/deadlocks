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
pthread_cond_t northC=PTHREAD_COND_INITIALIZER,eastC=PTHREAD_COND_INITIALIZER,southC=PTHREAD_COND_INITIALIZER,westC=PTHREAD_COND_INITIALIZER;
int ncurr = 0, ecurr = 0, scurr = 0, wcurr = 0;
int crossing = 0;
int active_carts = 0;


void arrive(train c){
    pthread_mutex_lock(&intersection);
    char* direction;
    switch(c.direction){
        case 'n':
            direction="North";
            pthread_cond_wait(&northQ,&intersection);
            ncurr = 1;
            break;
        case 'e':
            direction="East";
            pthread_cond_wait(&eastQ,&intersection);
            ecurr = 1;
            break;
        case 's':
            direction="South";
            pthread_cond_wait(&southQ,&intersection);
            scurr = 1;
            break;
        case 'w':
            direction="West";
            pthread_cond_wait(&westQ,&intersection);
            wcurr = 1;
            break;
        default:
            break;
    }
    printf("CART %d from %s arrives at crossing\n",c.trainID,direction);
    pthread_mutex_unlock(&intersection);    
    
}

void cross(train c){
    pthread_mutex_lock(&intersection);
    char* direction;
    switch(c.direction){
        case 'n':
            direction="North";
            if (wcurr == 1){
                pthread_cond_wait(&northC,&intersection);
            }
            sleep(2);
            ncurr = 0;
            pthread_cond_signal(&northQ);
            pthread_cond_signal(&eastC);
            break;
        case 'e':
            direction="East";
            if (ncurr == 1){
                pthread_cond_wait(&eastC,&intersection);
            }
            sleep(2);
            ecurr = 0;
            pthread_cond_signal(&eastQ);
            pthread_cond_signal(&southC);
            break;
        case 's':
            direction="South";
            if (ecurr == 1){
                pthread_cond_wait(&southC,&intersection);
            }
            sleep(2);
            scurr = 0;
            pthread_cond_signal(&southQ);
            pthread_cond_signal(&westC);
            break;
        case 'w':
            direction="West";
            if (scurr == 1){
                pthread_cond_wait(&westC,&intersection);
            }
            sleep(2);
            wcurr = 0;
            pthread_cond_signal(&westQ);
            pthread_cond_signal(&northC);
            break;
        default:
            break;
    }
    printf("CART %d from %s crossed\n",c.trainID,direction);
	pthread_mutex_unlock(&intersection);
}

void leave(train c){
    active_carts--;
}

//main function for each thread
void *manage_thread(void *c){
    train* cart = (train*)c; //set the data to a variable so it can be used
    //printf("Main:\tTrain %d\t Direction %c\n",cart->trainID,cart->direction);
    arrive(*cart);
    cross(*cart);
    leave(*cart);
}

int main (int argc, char* argv[]){
	pthread_t* trains = malloc((int)strlen(argv[1])*sizeof(pthread_t)); // create an array of threads for as many trains as given	
	train* cart = malloc((int)strlen(argv[1])*sizeof(train));
    char *str; // hold argv[1] to iterate through characters
    int id;
    active_carts = (int)strlen(argv[1]);
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

    int deadlock_counter = 0;
    while(active_threads != 0){
        if(ncurr == ecurr == scurr == wcurr == 1){
            printf("DEADLOCK MOTHERFUCKER!");
            int next = deadlock_counter%4;
            switch(next){
                case 0:
                    pthread_cond_signal(&northC);
                    break;
                case 1:
                    pthread_cond_signal(&eastC);
                    break;
                case 2:
                    pthread_cond_signal(&southC);
                    break;
                case 3:
                    pthread_cond_signal(&westC);
                    break;
                default:
                    break;
            }
            deadlock_counter++;
        }
    }

    int i;
    for(i=0;i<id;i++){pthread_join(trains[i], NULL);}
    pthread_cond_destroy(&northQ);pthread_cond_destroy(&eastQ);pthread_cond_destroy(&southQ);pthread_cond_destroy(&westQ);
    pthread_cond_destroy(&northC);pthread_cond_destroy(&eastC);pthread_cond_destroy(&southC);pthread_cond_destroy(&westC);
    pthread_mutex_destroy(&intersection);
    pthread_exit(NULL);
    return 0;
}
