//Daiana Kathrin Santana Santos 120.357
//Isadora Rosa de Freitas Muniz 120.431

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define TAM_TABULEIRO 2048
#define NUM_GERACOES 2000
#define SRAND_VALUE 1985

/*

Comando prompt pra executar:

$ gcc -fopenmp JogoDaVidaOMPCritical_AtivA.c
$ ./a.out

*/

int **Tabuleiro1, **Tabuleiro2;
int MAX_THREADS = 12;

void inicializa()
{

    int i, j;
    Tabuleiro1 = malloc(TAM_TABULEIRO*sizeof(int*)); // CRIA VETOR DE PONTEIROS
    Tabuleiro2 = malloc(TAM_TABULEIRO*sizeof(int*));

    srand(SRAND_VALUE); // GERA NUMEROS ALEATORIOS
    for(i=0; i<TAM_TABULEIRO; i++)   // CRIAR A MATRIZ
    {
        Tabuleiro1[i] = malloc(TAM_TABULEIRO*sizeof(int));// ALOCA ESPACO DE VETOR DE TAMANHO TAM_TABULEIRO PARA CADA PONTEIRO
        Tabuleiro2[i] = malloc(TAM_TABULEIRO*sizeof(int));

        for(j=0; j<TAM_TABULEIRO; j++)   // PREENCHER A MATRIZ
        {
            Tabuleiro1[i][j] = rand() % 2; // PREENCHE A MATRIZ COM OS VALORES ALEATÓRIOS 1 OU 0
            Tabuleiro2[i][j] = 0; // PREENCHE SEGUNDA MATRIZ SOMENTE COM ZERO
        }
    }
}

int getNeighbors(int i, int j, int** Tabuleiro)
{
    int l_esq, l_meio, l_dir, c_baix, c_meio, c_cim;
    int vivos = 0;

    if(j == 0) c_baix = TAM_TABULEIRO-1;
    else c_baix = j-1;

    c_meio = j;

    c_cim = (j+1) % TAM_TABULEIRO;


    if(i == 0) l_esq = TAM_TABULEIRO-1;
    else l_esq = i-1;

    l_meio = i;

    l_dir = (i+1) % TAM_TABULEIRO;

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
    int i, j, vivos;
    #pragma omp for private(i,j)

    for(i=0; i<TAM_TABULEIRO; i++)
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

int getVivos(int **Tabuleiro)
{
	
    int i, j;
    int vivos = 0;
	#pragma omp critical
    for(i=0; i<TAM_TABULEIRO; i++)
        for(j=0; j<TAM_TABULEIRO; j++)
            vivos += Tabuleiro[i][j];

    return vivos;
}

void jogoDaVida()
{

    int i;

    #pragma omp parallel shared(Tabuleiro1, Tabuleiro2) private(i) num_threads(MAX_THREADS)

    for(i=1; i<=NUM_GERACOES; i++)
    {
        if(i%2) //se i é impar
            setNewGeneration(Tabuleiro1, Tabuleiro2);
        else //se i é par
            setNewGeneration(Tabuleiro2, Tabuleiro1);
    }
}
int main()
{

    clock_t t; //variável para armazenar tempo
    t = clock(); //armazena tempo
    inicializa(); // CRIA E PREENCHE TRABULEIROS 1 E 2
    printf("Vivos inicial = %d\n", getVivos(Tabuleiro1)); // MOSTRA A QUANTIDADE DE VIVOS INICIAL

    double final;
    struct timeval inicio, final2;
    int tmp;
    int vivos;

    omp_set_num_threads(MAX_THREADS); // seta numero de threads



    gettimeofday(&inicio, NULL); //INCIA A CONTAGEM DO TEMPO


    jogoDaVida();// executa o jogo

    if(NUM_GERACOES%2 == 0)
        vivos = getVivos(Tabuleiro1);

    else 	vivos = getVivos(Tabuleiro2);

    printf("Vivos final = %d\n", vivos);


    gettimeofday(&final2, NULL);


    tmp = (int) (final2.tv_sec - inicio.tv_sec); // em segundos
    t = clock() - t; //tempo final - tempo inicial
    printf("\nTempo decorrido: %d s\n", tmp);
    printf("Tempo de execucao: %lf\n", ((double)t)/(CLOCKS_PER_SEC));


    return 0;
}



