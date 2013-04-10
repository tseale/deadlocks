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

// struct for train information
typedef struct{
	char direction;
	int trainID;
}train;

pthread_mutex_t intersection = PTHREAD_MUTEX_INITIALIZER; // mutex for the train intersection
pthread_cond_t northQ=PTHREAD_COND_INITIALIZER,eastQ=PTHREAD_COND_INITIALIZER,southQ=PTHREAD_COND_INITIALIZER,westQ=PTHREAD_COND_INITIALIZER; // condition variables for queue management
pthread_cond_t northC=PTHREAD_COND_INITIALIZER,eastC=PTHREAD_COND_INITIALIZER,southC=PTHREAD_COND_INITIALIZER,westC=PTHREAD_COND_INITIALIZER; // condition variables for right first rule
int ncurr = 0, ecurr = 0, scurr = 0, wcurr = 0; // if there is a cart at the crossing for any given direction
int active_carts = 0;

// handle how each train advances through the queue to arrive at the crossing
void arrive(train c){
    pthread_mutex_lock(&intersection); // lock the mutex
    char* direction; // to make printing nicer
    // handle each direction, carts not at the crossing are waiting for the cart ahead to signal that they have crossed, so this cart can arrive
    // *curr variables indicate whether or not a cart is at the crossing, adjust accordingly
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
    pthread_mutex_unlock(&intersection); // lock the mutex 
    
}

// handle crossing the intersection with a train
void cross(train c){
    pthread_mutex_lock(&intersection); // lock the mutex
    char* direction; // to make printing nicer
    // handle each direction
    /*
        North yields to West
        West yields to South
        South yields to East
        East yields to North
    */
    switch(c.direction){
        case 'n':
            direction="North";
            // if a cart is at the west, north yields to it
            // waits for the signla to go...
            if (wcurr == 1){
                pthread_cond_wait(&northC,&intersection);
            }
            pthread_cond_signal(&northQ); ncurr = 0; // send a signal that the next north can go, update the variable accordingly
            pthread_cond_signal(&eastC); // tell east it's chill to go now
            sleep(1);
            break;
        case 'e':
            direction="East";
            // if a cart is at the north, east yields to it
            // waits for the signla to go...
            if (ncurr == 1){
                pthread_cond_wait(&eastC,&intersection);
            }
            pthread_cond_signal(&eastQ); ecurr = 0; // send a signal that the next east can go, update the variable accordingly
            pthread_cond_signal(&southC); // tell south it's chill to go now
            sleep(1);
            break;
        case 's':
            direction="South";
            // if a cart is at the east, south yields to it
            // waits for the signla to go...
            if (ecurr == 1){
                pthread_cond_wait(&southC,&intersection);
            }
            pthread_cond_signal(&southQ); scurr = 0; // send a signal that the next south can go, update the variable accordingly
            pthread_cond_signal(&westC); // tell west it's chill to go now
            sleep(1);
            break;
        case 'w':
            direction="West";
            // if a cart is at the south, west yields to it
            // waits for the signla to go...
            if (scurr == 1){
                pthread_cond_wait(&westC,&intersection);
            }
            pthread_cond_signal(&westQ); wcurr = 0; // send a signal that the next west can go, update the variable accordingly
            pthread_cond_signal(&northC); // tell north it's chill to go now
            sleep(1);
            break;
        default:
            break;
    }
    printf("CART %d from %s leaving crossing\n",c.trainID,direction);
	pthread_mutex_unlock(&intersection); // unlock the mutex
}

void leave(){
    pthread_mutex_lock(&intersection);
    active_carts--;
    pthread_mutex_unlock(&intersection);
}

//main function for each thread
void *manage_thread(void *c){
    train* cart = (train*)c; //set the data to a variable so it can be used
    arrive(*cart);
    cross(*cart);
    leave();
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

    // check for deadlock every 3 seconds
    while(active_carts != 0){
        sleep(3);
        if(ncurr==1 && ecurr==1 && scurr==1 && wcurr==1){
            char* direction;
            // alternate which cart we give priority to cross
            int next = deadlock_counter%4; // so the counter always is 0-3
            switch(next){
                case 0:
                    direction="North";
                    pthread_cond_signal(&northC);
                    break;
                case 1:
                    direction="East";
                    pthread_cond_signal(&eastC);
                    break;
                case 2:
                    direction="South";
                    pthread_cond_signal(&southC);
                    break;
                case 3:
                    direction="West";
                    pthread_cond_signal(&westC);
                    break;
                default:
                    break;
            }
            printf("DEADLOCK: CART jam detected, signalling %s to go\n",direction);
            deadlock_counter++;
        }
    }

    // join and destroy everything
    int i;
    for(i=0;i<id;i++){pthread_join(trains[i], NULL);}
    pthread_cond_destroy(&northQ);pthread_cond_destroy(&eastQ);pthread_cond_destroy(&southQ);pthread_cond_destroy(&westQ);
    pthread_cond_destroy(&northC);pthread_cond_destroy(&eastC);pthread_cond_destroy(&southC);pthread_cond_destroy(&westC);
    pthread_mutex_destroy(&intersection);
    pthread_exit(NULL);
    return 0;
}
