//
//  main.c
//  pacotes_lista_ajd
//
//  Created by Pedro Manhez Naresi on 6/9/16.
//  Copyright © 2016 Pedro Manhez Naresi. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#define MAX 100;



long int vertices[100];
int quantidade_de_vertices;

int converteVertice(long int valor_do_vertice){
    int i;
    for(i = 0; i < quantidade_de_vertices; i++){
        if(vertices[i] == valor_do_vertice){
            quantidade_de_vertices++;
            return i;
        }
    }
    return -1;
}

int insere_vertice_para_converter(long int valor_do_vertice){
    vertices[quantidade_de_vertices] = valor_do_vertice;
    quantidade_de_vertices++;
    return (quantidade_de_vertices - 1);
}



typedef int TVertice;

typedef int TAresta;

typedef struct{
    int vert;
    struct TCelula *proximo;
}TCelula;

typedef struct{
    int tamanho;
    TCelula *primeiro;
    TCelula *ultimo;
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
        pgrafo->Adj[u-1]->ultimo->proximo=(TCelula*)nova;
        pgrafo->Adj[u-1]->ultimo=nova;
    }
    pgrafo->Adj[u-1]->tamanho++;
    
}
void tgrafo_inicia(TGrafo *pgrafo, int NVertices){
    int i;
    quantidade_de_vertices = 1;  //inicializa aqui o tamanho da lista de conversão
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

void verifica_dependencia(TGrafo *pgrafo){
    int i,j,k;
    TCelula *aux;
    for(i=0;i<pgrafo->NVertices;i++){
        k=0;
        printf("%d ", i+1);
        for(j=0;j<pgrafo->NVertices;j++){
            aux=pgrafo->Adj[j]->primeiro;
            while(aux!=NULL){
                if(aux->vert-1==i)
                    k++;
                aux=(TCelula*)aux->proximo;
            }
        }
        printf("%d ", k);
        k=0;
        printf("%d ", pgrafo->Adj[i]->tamanho);
        aux=pgrafo->Adj[i]->primeiro;
        while(aux!=NULL){
            printf("%d ", aux->vert);
            aux=(TCelula*)aux->proximo;
        }
        printf("\n");
    }
}



int main(){
    int N, D;
    long int aux1, aux2;
    TGrafo grafo;
    scanf("%d %d", &N, &D);
    tgrafo_inicia(&grafo, N);
    int i;
    for(i=0;i<D;i++){
        scanf("%ld %ld", &aux1, &aux2);
        int k = insere_vertice_para_converter(aux1);
        int q = insere_vertice_para_converter(aux2);
        tgrafo_inserearesta(&grafo, k, q);
    }
    verifica_dependencia(&grafo);
    return 0;
   
}
