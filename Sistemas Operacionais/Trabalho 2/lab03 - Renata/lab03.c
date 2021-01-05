/*
 Sistemas Operacionais - Noturno - 2018
 Laboratório 3 - Deadlock

 Renata Sendreti Broder            RA: 112347
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <dlfcn.h>
#include <sys/syscall.h>

/*
Estruturas que serão utilizadas para constituir o grafo
*/
// tipoVertice é usada tanto para compor a lista de vértices do grafo como a lista adjacente a cada vértice
typedef struct tipoLista tipoLista;

typedef struct tipoVertice{
    int id;
    sem_t *sem;
    pthread_t proc;
    int tipo;
    tipoLista *listaAdj;
    int grau;
    int visitado;
    struct tipoVertice *prox;
}tipoVertice;

// tipoLista é usada tanto como a lista de vértices do grafo quanto como lista de vértices adjacentes a cada vértice
typedef struct tipoLista{
    tipoVertice *inicio, *fim;
}tipoLista;

// tipoGrafo é usada para guardar o grafo de recursos e processos
typedef struct tipoGrafo{
    tipoLista *listaVertices;
    int tam;
}tipoGrafo;

/*
Funções auxiliares para inicialização de listas e grafo
*/    
void inicializaLista(tipoLista *lista){
    lista->inicio = NULL;
    lista->fim = NULL;
}

tipoGrafo* inicializaGrafo(tipoGrafo *grafo){
    grafo = malloc(sizeof(tipoGrafo));
    grafo->tam = 0;
    grafo->listaVertices = malloc(sizeof(tipoLista));
    inicializaLista(grafo->listaVertices);
    return grafo;
}

/*
Funções auxiliares para encontrar o id de um vértice a partir de pthread_t/sem_t ou o próprio vértice, a partir do id
*/
int getId(tipoGrafo *grafo, pthread_t proc, sem_t *sem){
    tipoVertice *percorre = grafo->listaVertices->inicio;
    if(sem == NULL){ //se é processo, compara com pthread_t
        while(percorre != NULL){
            if(percorre->proc == proc) return percorre->id;
            percorre = percorre->prox;
        }
    }
    else if(proc == -1){ //se é recurso, compara com sem_t
        while(percorre != NULL){
            if(percorre->sem == sem) return percorre->id;
            percorre = percorre->prox;
        }
    }
    return -1; //se não foi encontrado na lista de vértices, retorna -1 para criar um novo vértice
}

tipoVertice* getVertice(tipoGrafo *grafo, int id){
    tipoVertice *percorre = grafo->listaVertices->inicio;
    while(percorre != NULL){
        if(percorre->id == id) return percorre; //retorna o vértice correspondente ao id passado
        percorre = percorre->prox;
    }
    return NULL; //se vértice correspondente ao id não foi encontrado, retorna NULL
}

/*
Funções auxiliares para inserir vértices na lista de vértices do grafo ou na lista adjacente a cada vértice
*/ 
void insereLista(tipoLista *lista, tipoVertice vert){
    tipoVertice *aux = (tipoVertice*)malloc(sizeof(tipoVertice));
    *aux = vert;

    if(lista->inicio == NULL) lista->inicio = aux;
    else lista->fim->prox = aux;

    aux->prox = NULL;
    lista->fim = aux;
}

int insereVertice(tipoGrafo *grafo, pthread_t proc, sem_t *sem, int tipo){
    int id = getId(grafo, proc, sem); //busca id
    
    if(id == -1){ //se vertice do processo ainda não existe, cria e insere na lista de vértices do grafo
        tipoVertice vert;
        vert.id = grafo->tam;
        vert.grau = 0;
        vert.proc = proc;
        vert.sem = sem;
        vert.tipo = tipo;
        vert.listaAdj = malloc(sizeof(tipoLista));
        inicializaLista(vert.listaAdj);
        insereLista(grafo->listaVertices, vert);
        grafo->tam++;
        id = vert.id;
    }
    return id;
}

void insereAresta(tipoGrafo *grafo, int v1, int v2){
    tipoVertice *vert1 = getVertice(grafo, v1);
    tipoVertice *vert2 = getVertice(grafo, v2);
    vert1->grau++;
    vert2->grau++;
    insereLista(vert1->listaAdj, *vert2);
}

/*
Funções auxiliares para remover vértices da lista de vértices do grafo ou da arestas de um vértice
*/ 
void removeDaLista(tipoLista *lista, tipoVertice vert){
    tipoVertice *percorre = lista->inicio;

    if(lista->inicio != NULL){
        if(percorre->id == vert.id){ //se for o primeiro da lista
            lista->inicio = lista->inicio->prox;
            free(percorre);
            return;
        }
        while(percorre->prox != NULL && percorre->prox->id != vert.id)
            percorre = percorre->prox;
        if(percorre->prox != NULL){
            tipoVertice *r = percorre->prox;
            percorre->prox = r->prox;
            free(r);
        }
    }
}

void removeAresta(tipoGrafo *grafo, int v1, int v2){
    tipoVertice *vert1 = getVertice(grafo, v1);
    tipoVertice *vert2 = getVertice(grafo, v2);
    removeDaLista(vert1->listaAdj, *vert2);
    vert1->grau--;
    vert2->grau--;
}

/*
Funções para buscar ciclos em grafo, se o retorno for 1, há ciclos e não será possível chamar a original sem_wait()
*/
int buscaCiclo(tipoGrafo *grafo, int id, int idVertice){
    tipoVertice *vert = getVertice(grafo, id);
    vert->visitado = 1;

    tipoVertice *percorre = vert->listaAdj->inicio;
    while(percorre != NULL){
        tipoVertice *p = getVertice(grafo, percorre->id);
        
        if(p->id == idVertice) return 1;

        if(p->visitado == 0){
            if(buscaCiclo(grafo, p->id, idVertice) == 1) return 1;
        }
        p = p->prox;
    }
    return 0;
}
    
int inicializaBuscaCiclo(tipoGrafo *grafo, int vertice){
    tipoVertice *inicio = grafo->listaVertices->inicio;
    tipoVertice *percorre = getVertice(grafo, vertice);    
    
    while(inicio != NULL){
        inicio->visitado = 0;
        inicio = inicio->prox;
    }
    
    inicio = percorre->listaAdj->inicio;
    while(inicio != NULL){
        if(buscaCiclo(grafo, inicio->id, vertice) == 1) return 1;
        inicio = inicio->prox;
    }
    return 0;
}

tipoGrafo *grafo = NULL;
sem_t *sem = NULL;
int (*_sem_wait)(sem_t *) = NULL;
int (*_sem_post)(sem_t *) = NULL;

int sem_wait(sem_t *sem){
	if(sem == NULL){ //primeira vez que este recurso é requisitado
		sem = malloc(sizeof(sem_t));
		sem_init(sem, 0, 1); //inicialização do semáforo, só aceita 1 na região crítica
	}

    if(_sem_wait == NULL) _sem_wait = dlsym(RTLD_NEXT, "sem_wait"); //aponta para a sem_wait original
    if(grafo == NULL) grafo = inicializaGrafo(grafo);
    
    //identificação do processo e do recurso em questão
    pthread_t idProcesso = pthread_self();
    sem_t* idRecurso = sem;
    
    //insere o processo e o recurso como vértices no grafo, se eles já não estiverem lá (grafo existe para identificar ciclos), e retorna o identificador de cada um deles
    int proc = insereVertice(grafo, idProcesso, NULL, 0);
    int rec = insereVertice(grafo, -1, idRecurso, 1);
    insereAresta(grafo, proc, rec); //insere aresta processo->recurso

    if(inicializaBuscaCiclo(grafo, proc)) return -2; //se houver ciclos, não chama a sem_wait original e retorna erro
    int r = _sem_wait(sem); //chama a sem_wait original

    insereAresta(grafo, rec, proc); //quando passar pela requisição da sem_wait, cria aresta recurso->processo
    removeAresta(grafo, proc, rec); //processo ja utilizou recurso

    if(inicializaBuscaCiclo(grafo, proc)) return -2; //se houver ciclos, retorna erro
    return r; //se não houve ciclos, repassa o retorno da sem_wait original
}

int sem_post(sem_t *sem){
    if(_sem_post == NULL) _sem_post = dlsym(RTLD_NEXT, "sem_post"); //aponta para a sem_post original

    //identificação do processo e do recurso em questão
    pthread_t idProcesso = pthread_self();
    sem_t* idRecurso = sem; 

    //chama inserirVertice() para pegar id para remover aresta
    int proc = insereVertice(grafo, idProcesso, NULL, 0);
    int rec = insereVertice(grafo, -1, idRecurso, 1);
    removeAresta(grafo, rec, proc);

    return _sem_post(sem);
}