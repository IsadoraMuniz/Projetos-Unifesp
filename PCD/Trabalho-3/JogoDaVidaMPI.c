//Daiana Kathrin Santana Santos 120.357
//Isadora Rosa de Freitas Muniz 120.431

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define TAM_TABULEIRO 2048
#define NUM_GERACOES 2000
#define SRAND_VALUE 1985

int **Tabuleiro1, **Tabuleiro2;
int idProcesso, numprocs;
int linhas;


void inicializa()
{

    linhas = TAM_TABULEIRO/numprocs + (TAM_TABULEIRO % numprocs > idProcesso);

    Tabuleiro1 = malloc((linhas+2)*sizeof(int*));
    Tabuleiro2 = malloc((linhas+2)*sizeof(int*));

    int i, j;

    srand(SRAND_VALUE); // GERA NUMEROS ALEATORIOS
    for(i=0; i<=linhas+1; i++)  //vai até linhas porque adicionamos 2
    {
        Tabuleiro1[i] = malloc(TAM_TABULEIRO*sizeof(int));// ALOCA ESPACO DE VETOR DE TAMANHO TAM_TABULEIRO PARA CADA PONTEIRO
        Tabuleiro2[i] = malloc(TAM_TABULEIRO*sizeof(int));
        //printf("valor l=%d\n",idProcesso);

        for(j=0; j<TAM_TABULEIRO; j++)  // PREENCHER A MATRIZ
        {
            if(i == 0 || i == linhas+1) Tabuleiro1[i][j] = 0;
            else Tabuleiro1[i][j] = rand() % 2;// PREENCHE A MATRIZ COM OS VALORES ALEATÓRIOS 1 OU 0
            Tabuleiro2[i][j] = 0;// PREENCHE SEGUNDA MATRIZ SOMENTE COM ZERO
        }
    }
}

void recebeEnviaDados(int **Tabuleiro)
{

    int ant = idProcesso == 0 ? numprocs-1 : idProcesso-1;
    int prox = (idProcesso + 1) % numprocs;

    MPI_Request request;

    MPI_Isend(Tabuleiro[1], TAM_TABULEIRO, MPI_INT, ant, 0, MPI_COMM_WORLD, &request);
    MPI_Isend(Tabuleiro[linhas], TAM_TABULEIRO, MPI_INT, prox, 0, MPI_COMM_WORLD, &request);
    MPI_Recv(Tabuleiro[linhas+1], TAM_TABULEIRO, MPI_INT, prox, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(Tabuleiro[0], TAM_TABULEIRO, MPI_INT, ant, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


}

void copia(int **Tabuleiro)
{
    int i;
    for(i=0; i<TAM_TABULEIRO; i++)
    {
        Tabuleiro[0][i] = Tabuleiro[linhas][i];
        Tabuleiro[linhas+1][i] = Tabuleiro[1][i];
    }
}

int getNeighbors(int i, int j, int** Tabuleiro)
{
    int l_esq, l_meio, l_dir, c_baix, c_meio, c_cim;
    int vivos = 0;

    l_esq = i-1;
    l_meio = i;
    l_dir = i+1;

    if(j == 0) c_baix = TAM_TABULEIRO-1;
    else c_baix = j-1;
    c_meio = j;
    c_cim = (j+1) % TAM_TABULEIRO;

    vivos += Tabuleiro[l_esq][c_baix];
    vivos += Tabuleiro[l_meio][c_baix];
    vivos += Tabuleiro[l_dir][c_baix];
    vivos += Tabuleiro[l_dir][c_meio];
    vivos += Tabuleiro[l_dir][c_cim];
    vivos += Tabuleiro[l_meio][c_cim];
    vivos += Tabuleiro[l_esq][c_cim];
    vivos += Tabuleiro[l_esq][c_meio];

    return vivos;
}

void setNewGeneration(int **velha, int **nova)
{

    if(numprocs > 1) recebeEnviaDados(velha);
    else copia(velha);

    int i, j, vivos;
    for(i=1; i<=linhas; i++)
    {
        for(j=0; j<TAM_TABULEIRO; j++)
        {
            vivos = getNeighbors(i, j, velha); // VE A QUANTIDADE DE VIVOS NOS 8 VIZINHOS

            if(velha[i][j] && vivos < 2) nova[i][j] = 0; // Células vivas com menos de 2 (dois) vizinhas vivas morrem por abandono
            else if(velha[i][j] && (vivos == 3 || vivos == 2)) nova[i][j] = 1; //Cada célula viva com 2 (dois) ou 3 (três) vizinhos deve permanecer viva para a próxima geração
            else if(velha[i][j] && vivos >= 4) nova[i][j] = 0; //Cada célula viva com 4 (quatro) ou mais vizinhos morre por superpopulação.
            else if(!velha[i][j] && vivos == 3) nova[i][j] = 1; //Cada célula morta com exatamente 3 (três) vizinhos deve se tornar viva.
            else nova[i][j] = 0;
        }
    }
}

int getVivos(int **Tabuleiro) // CONTA QUANTIDADE DE VIVOS NO TABULEIRO
{

    int i, j;
    int vivos = 0;

    for(i=1; i<linhas+1; i++)
        for(j=0; j<TAM_TABULEIRO; j++)
            vivos += Tabuleiro[i][j];

    return vivos;
}

void jogoDaVida()
{

    int i;
    for(i=1; i<=NUM_GERACOES; i++)
    {
        if(i%2) //se i é impar
            setNewGeneration(Tabuleiro1, Tabuleiro2);
        else //se i é par
            setNewGeneration(Tabuleiro2, Tabuleiro1);
    }
}

int main(int argc, char *argv[])
{
    struct timeval tmp_inicio, tmp_final;
    int tmp, vivos, total;
    gettimeofday(&tmp_inicio, NULL);//INCIA A CONTAGEM DO TEMPO

    MPI_Init(&argc, &argv); //Inicializa a execução MPI
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs); //Define numero de processos
    MPI_Comm_rank(MPI_COMM_WORLD, &idProcesso); //Atribui ID do processo

    inicializa(); // CRIA E PREENCHE TRABULEIROS 1 E 2
    jogoDaVida();

    if(NUM_GERACOES%2 == 0)
    {
        vivos = getVivos(Tabuleiro1);
    }
    else
    {
        vivos = getVivos(Tabuleiro2);
    }


    MPI_Reduce(&vivos, &total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if(idProcesso == 0)
        printf("final = %d\n", total);

    MPI_Finalize();

    gettimeofday(&tmp_final, NULL);
    tmp = (int) ( (tmp_final.tv_sec - tmp_inicio.tv_sec) );
    printf("\nTempo decorrido: %d s\n", tmp);

    return 0;
}
