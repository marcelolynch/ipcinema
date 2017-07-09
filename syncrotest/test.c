#include "synchronization.h"
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

static void critical_zone(){
	int t = rand()%5 + 1;
	sleep(t);
}

static void * thread_work(void* data){
	while(1){
		int t = rand()%3 + 1;

		sleep(t);

		int choice = rand()%100;
		if(choice < 10){
			printf("   A wild writer appears!\n");
			enter_critical(WRITER);
				critical_zone();
			leave_critical(WRITER);
		} else{
			enter_critical(READER);
				critical_zone();
			leave_critical(READER);
		}
	}
}



int main(void){
	srand(time(0));

	synchro_init();
	int i = 0;
	pthread_t threads[15];

	while(i < 15){
    	pthread_create(&threads[i++], NULL, thread_work, NULL); 	
	}

	i=0;
	while(i < 15){
		pthread_join(threads[i++], NULL);
	}

	return 0;
}


