//Everton da Silva Coelho  101937
//Gabriel Borin Takahashi 101953
//Pedro Manhez Naresi 105615

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

sem_t mutex1;
sem_t mutex2;

int T, U;

void *thread1_func(void *arg){
	while(1){
		printf("ID DO SEMAFORO: %lu, ID DA THREAD: %ld\n", (size_t)&mutex1, pthread_self());
		sem_wait(&mutex1);
		printf("T1 em execucao\n");
		//printf("ID DO SEMAFORO: %lu, ID DA THREAD: %ld\n", (size_t)mutex1, pthread_self());
		sem_wait(&mutex2);

		sem_post(&mutex1);
		sem_post(&mutex2);
	}
	int *retorno = (int*) malloc(sizeof(int));
	*retorno = 0;
	pthread_exit(retorno);
}

void *thread2_func(void *arg){
	while(1){
		printf("ID DO SEMAFORO: %lu, ID DA THREAD: %ld\n", (size_t)&mutex2, pthread_self());
		sem_wait(&mutex2);
		printf("T2 em execucao\n");
		//printf("ID DO SEMAFORO: %lu, ID DA THREAD: %ld\n", (size_t)mutex2, pthread_self());
		sem_wait(&mutex1);

		sem_post(&mutex1);
		sem_post(&mutex2);
	}
	int *retorno = (int*) malloc(sizeof(int));
	*retorno = 0;
	pthread_exit(retorno);
}

int main(void) {
	pthread_t threads[2];

	sem_init(&mutex1, 0, 1);
	sem_init(&mutex2, 0, 1);

	pthread_create(&(threads[0]), NULL, thread1_func, NULL);
	pthread_create(&(threads[1]), NULL, thread2_func, NULL);

	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);

	return 0;
}
