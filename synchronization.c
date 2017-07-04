#include "synchronization.h"

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

pthread_mutex_t w_mtx; // Controla el acceso a writers
int writers;

pthread_mutex_t r_mtx; // Controla el acceso a readcount
int readcount;


pthread_mutex_t g_mtx; // Zona critica

sem_t shortwait;	//Los semaforos actuan como colas
sem_t longwait;

// Inicializo los mutex y semaforos
void synchro_init(){
	pthread_mutex_init(&w_mtx, NULL);
	pthread_mutex_init(&r_mtx, NULL);
	pthread_mutex_init(&g_mtx, NULL);

	sem_init(&shortwait, 0, 1);
	sem_init(&longwait, 0, 1);

	writers = 0;
	readcount = 0;
}

static void enter_writer(){
	pthread_mutex_lock(&w_mtx);
		writers++;
		if(writers == 1){
			//Espero en la cola corta, por ahi hay lectores
			sem_wait(&shortwait);
		}
	pthread_mutex_unlock(&w_mtx);

	pthread_mutex_lock(&g_mtx); // Espero a que terminen los que estan adentro (escritores o lectores)
}


static void leave_writer(){
	pthread_mutex_unlock(&g_mtx);

	pthread_mutex_lock(&w_mtx);
		writers--;
		if(writers == 0){ 			//Pueden entrar el siguiente lector
			sem_post(&shortwait);
		}
	pthread_mutex_unlock(&w_mtx); 
}

static void enter_reader(){
	sem_wait(&longwait); // Se encolan varios lectores que esperan
	sem_wait(&shortwait); // Aca a lo sumo hay un lector esperando (y muchos escritores)

	//Pude entrar
	pthread_mutex_lock(&r_mtx); //Acceso a readcount
		readcount++;
		if(readcount == 1){ 			//Soy el primer lector
			pthread_mutex_lock(&g_mtx); //Entonces espero a que terminen los lectores
		}
										//Si no soy el primer lector ya hay alguno adentro, entro directo
	pthread_mutex_unlock(&r_mtx);
	
	sem_post(&shortwait); 				// Entrara el proximo de la cola corta (si es un escritor, solo yo entro a la zona critica
										// y el resto de los lectores se encolaran hasta que termine ese).
	sem_post(&longwait); 				// Dejo entrar al proximo lector a la cola corta 
}


static void leave_reader(){
	pthread_mutex_lock(&r_mtx);
		readcount--;
		if(readcount == 0){
			pthread_mutex_unlock(&g_mtx); //Puede entrar quien este esperando
		}
	pthread_mutex_unlock(&r_mtx);
}	

void enter_critical(synchro_type type){
	if(type == READER){
		enter_reader();
		#ifdef DEBUG
		printf("Enter reader\n");
		#endif
	}
	else{
		enter_writer();
		#ifdef DEBUG
		printf("Enter writer\n");
		#endif
	}
}

void leave_critical(synchro_type type){
	if(type == READER){
		#ifdef DEBUG
		printf("Reader leaving\n");
		#endif
		leave_reader();
	}
	else{
		#ifdef DEBUG
		printf("Writer leaving\n");
		#endif
		leave_writer();
	}
}