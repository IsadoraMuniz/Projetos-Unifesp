//Everton da Silva Coelho  101937
//Gabriel Borin Takahashi 101953
//Pedro Manhez Naresi 105615

//backtrace: pegar end da pilha do processo
// ou phread_self

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <signal.h>

#define MaxNumVertices  100
#define MaxNumArestas   4500

typedef int bool;
#define false           0
#define true            1

#define MAX 100;


typedef int TVertice;

typedef int TAresta;

typedef struct Cel TApontadorLista;

typedef struct Cel{
    int vert;
    TApontadorLista *proximo;
}TCelula;

typedef struct{
    int tamanho;
    TApontadorLista *primeiro;
    TApontadorLista *ultimo;
}TLista;

typedef struct{
    TVertice vertice;
    TAresta aresta;
}TAdjacencia;

typedef struct{
    TLista *Adj[100];
    int NVertices;
    int NArestas;
}TGrafo;

typedef struct Celula TApontadorPilha;

typedef struct Celula{
    int item;
   TApontadorPilha *proximo;
}TCelulaPilha;

typedef struct Pilha{
    TApontadorPilha *topo;
}TPilha;

//Global variables
long vertices[MaxNumVertices];
int quantidade_de_vertices = 0;
TGrafo grafo;
int aux = 0;

TLista *tlista_inicia(){
    TLista *pLista = (TLista*)malloc(sizeof(TLista));
    pLista->primeiro=NULL;
    pLista->ultimo=NULL;
    pLista->tamanho=0;
    return pLista;
}

void lista_inicia(TGrafo *pgrafo, int nvertices){
    int i;
    for(i=0;i<nvertices;i++){
        pgrafo->Adj[i]=tlista_inicia();
    }
}

void lista_insere(TGrafo *pgrafo, int u, int v){
    TCelula *nova = (TCelula*)malloc(sizeof(TCelula));
    nova->vert=v;
    nova->proximo=NULL;
    if(pgrafo->Adj[u-1]->tamanho == 0)
    {
        pgrafo->Adj[u-1]->primeiro=nova;
        pgrafo->Adj[u-1]->ultimo=nova;
    }
    else
    {
        pgrafo->Adj[u-1]->ultimo->proximo=nova;
        pgrafo->Adj[u-1]->ultimo=nova;
    }
    pgrafo->Adj[u-1]->tamanho++;

}
void tgrafo_inicia(TGrafo *pgrafo, int NVertices){
    int i;

    pgrafo->NVertices=NVertices;
    pgrafo->NArestas=0;

    for(i=0;i<pgrafo->NVertices;i++){
        pgrafo->Adj[i]=tlista_inicia();
    }

}

int tgrafo_inserearesta(TGrafo *pgrafo, int u, int v){
    lista_insere(pgrafo, u, v);
    pgrafo->NArestas++;
    return 0;
}

TPilha* criaPilha(void){
    TPilha *p = (TPilha*)malloc(sizeof(TPilha));
    p->topo=NULL;
    return p;
}

int PilhaEhVazia(TPilha* p){
    if(p->topo==NULL)
        return 1;
    else
        return 0;
}

void Empilha(TPilha* p, int v){
    TCelulaPilha *novo = (TCelulaPilha*)malloc(sizeof(TCelulaPilha));
    novo->item=v;
    novo->proximo=p->topo;
    p->topo=novo;
}

float Desimpilha(TPilha* p){
    float v;
    TCelulaPilha* aux;
    if(PilhaEhVazia(p)==1){
        exit(1);
    }else{
        aux=p->topo;
        v=aux->item;
        p->topo=aux->proximo;
        free(aux);
        return v;
    }

}

bool dfs(int v)
{
	TPilha *pilha = criaPilha();
	bool visitados[grafo.NVertices], pilha_rec[grafo.NVertices];

	// inicializa visitados e pilha_rec com false
	for(int i = 0; i < grafo.NVertices; i++)
		visitados[i] = pilha_rec[i] = false;

	// faz uma DFS
	while(true)
	{
		bool achou_vizinho = false;
		TCelula *aux;

		if(!visitados[v]) {
			Empilha(pilha, v);
			visitados[v] = pilha_rec[v] = true;
		}

		aux = grafo.Adj[v]->primeiro;
		while(aux != grafo.Adj[v]->ultimo)
		{
			// se o vizinho já está na pilha é porque existe ciclo
			if(pilha_rec[aux->vert])
				return true;
			else if(!visitados[aux->vert])
			{
				// se não está na pilha e não foi visitado, indica que achou
				achou_vizinho = true;
				break;
			}
			aux = aux->proximo;
		}

		if(!achou_vizinho)
		{
			pilha_rec[pilha->topo->item] = false; // marca que saiu da pilha
			Desimpilha(pilha); // remove da pilha
			if(PilhaEhVazia(pilha))
				break;
			v = pilha->topo->item;
		}
		else
			v = grafo.Adj[v]->ultimo->vert;
	}

	return false;
}

bool temCiclo()
{
	for(int i = 0; i < grafo.NVertices; i++)
	{
		if(dfs(i))
			return true;
	}
	return false;
}

int converteVertice(long valor_do_vertice){
    int i;
    for(i = 0; i < quantidade_de_vertices; i++){
        if(vertices[i] == valor_do_vertice){
            quantidade_de_vertices++;
            return i;
        }
    }
    quantidade_de_vertices++;
    vertices[i] = valor_do_vertice;
    //printf("HASH %d\n", i);
    return i;
}


int (*_sem_wait)(sem_t *) = NULL;

int sem_wait(sem_t *sem) {
	int r;
    if (aux == 0){
        aux = 1;
        tgrafo_inicia(&grafo, MaxNumVertices);
    }
	if (!_sem_wait) {
		_sem_wait = dlsym(RTLD_NEXT, "sem_wait");
		/* Irá apontar para o sem_wait original*/
	}
	else if (temCiclo() == false) {
		tgrafo_inserearesta(&grafo, converteVertice((size_t)sem), converteVertice(pthread_self()));
		printf("\t Semaforo %ld na thread %ld\n", (size_t)sem, pthread_self());
		//printf("\t Dentro da sem_wait()... faça o que quiser aqui!\n");
		r = _sem_wait(sem);
		return(r);
	}else{
		printf("\t Encontrou deadlock\n");
		exit(127);
        return 0;
	}
}
