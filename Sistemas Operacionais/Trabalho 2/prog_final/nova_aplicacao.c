#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

sem_t mutex1;//CRIACAO DA VARIAVEL 1 DO TIPO SEMAFORO
sem_t mutex2;//CRIACAO DA VARIAVEL 2 DO TIPO SEMAFORO


void *thread_1(void *arg){
	while(1){
		printf("Execucao Thread 1\n");
		printf("ID semaforo = %lu, ID thread =  %ld\n", (size_t)&mutex1, pthread_self());//IMPRIME INFORMACOES DO SEMAFORO E DA THREAD EM EXECUCAO
		sem_wait(&mutex1);// FUNCAO PRINCIPAL DE VERIFICACAO DA OCORRENCIA DE CICLO --> DEADLOCK
		sem_wait(&mutex2); // FUNCAO PRINCIPAL DE VERIFICACAO DA OCORRENCIA DE CICLO --> DEADLOCK
		sem_post(&mutex2); // FUNCAO QUE INCREMENTA (DESBLOQUEIA) O SEMAFORO mutex2
		sem_post(&mutex1); // FUNCAO QUE INCREMENTA (DESBLOQUEIA) O SEMAFORO mutex1 
		
	}
	printf("Termina Thread1\n");
	
	pthread_exit(NULL);// FUNCAO QUE TERMINA O THREAD DE CHAMADA

	
}

void *thread_2(void *arg){
	while(1){
		printf("Execucao Thread 2\n");
		printf("ID semaforo = %lu, ID thread =  %ld\n", (size_t)&mutex2, pthread_self());//IMPRIME INFORMACOES DO SEMAFORO E DA THREAD EM EXECUCAO
		
		sem_wait(&mutex2);// FUNCAO PRINCIPAL DE VERIFICACAO DA OCORRENCIA DE CICLO --> DEADLOCK
		sem_wait(&mutex1);// FUNCAO PRINCIPAL DE VERIFICACAO DA OCORRENCIA DE CICLO --> DEADLOCK
		sem_post(&mutex1);// FUNCAO QUE INCREMENTA (DESBLOQUEIA) O SEMAFORO mutex1
		sem_post(&mutex2);// FUNCAO QUE INCREMENTA (DESBLOQUEIA) O SEMAFORO mutex2
		
		
	}
	printf("Termina Thread2\n");
	
	pthread_exit(NULL);// FUNCAO QUE TERMINA O THREAD DE CHAMADA
	
}


int main(void) {
	pthread_t threads[2];// CRIA VETOR DE THREADS, COM DUAS POSICOES

	printf("/*************************************************/\n");
	printf("/*************************************************/\n");
	printf("/***UNIFESP - Instituto de Ciência e Tecnologia***/\n");
	printf("/***************2020-1ºSEMESTRE*******************/\n");
	printf("/************Sistemas Operacionais****************/\n");
	printf("/******Daiana Kathrin Santana Santos 120.357******/\n");
	printf("/******Isadora Rosa de Freitas Muniz 120.431******/\n");
	printf("/*************************************************/\n");
	printf("/*************************************************/\n\n\n");

	sem_init(&mutex1, 0, 1);// FUNCAO QUE INICIALIZA O SEMAFORO mutex1, COM VALOR INICIAL 1.O VALOR 0 DO SEGUNDO ARGUMENTO DA FUNCAO PERMITE O COMPARTILHAMENTO ENTRE OS THREADS DE UM PROCESSO, E DEVEM ESTAR LOCALIZADOS EM ALGUM ENDEREÇO QUE SEJA VISIVEL PARA TODOS OS TOPICOS (VARIAVEL GLOBAL OU UMA VARIAVEL ALOCADA DINAMICAMENTE NA PILHA)
	sem_init(&mutex2, 0, 1);// FUNCAO QUE INICIALIZA O SEMAFORO mutex2, COM VALOR INICIAL 1.
	pthread_create(&(threads[0]), NULL, thread_1, NULL);//CRIA A THREAD NA POSICAO 0 DO VETOR DE THREADS
	pthread_create(&(threads[1]), NULL, thread_2, NULL);//CRIA A THREAD NA POSICAO 1 DO VETOR DE THREADS
	pthread_join(threads[0], NULL);// ESPERA PELO THREAD NA POSICAO 0 TERMINAR, E A FUNCAO DEVE SER JUNTAVEL
	pthread_join(threads[1], NULL);// ESPERA PELO THREAD NA POSICAO 1 TERMINAR, E A FUNCAO DEVE SER JUNTAVEL

	return 0;
}
