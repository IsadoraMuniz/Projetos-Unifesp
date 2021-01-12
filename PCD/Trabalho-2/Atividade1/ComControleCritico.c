//Daiana Kathrin Santana Santos 120.357
//Isadora Rosa de Freitas Muniz 120.431

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 15

int NumThreads, SOMA=0, respond=0, request=0;

void CalculaSoma()
{
    int local = SOMA;
    sleep(rand()%2);
    SOMA = local + 1;
}

void cliente(int i)
{
    
    while(SOMA < N)
    {
        while(respond != i)
            request = i;
        CalculaSoma();
        respond = 0;
    }
}

void servidor()
{
    while(SOMA < N)
    {
        while(request == 0);
        respond = request;
        while(respond != 0);
        request = 0;
        printf("SOMA = %d\n", SOMA);
    }
}

int main(int argc, char *argv[])
{

    NumThreads = atoi(argv[1]);
    printf("Numero de threads: %d\n", NumThreads);

    omp_set_num_threads(NumThreads);

    #pragma omp parallel
    if(omp_get_thread_num() == 0)
        servidor();
    else
        cliente(omp_get_thread_num());


    return 0;
}

