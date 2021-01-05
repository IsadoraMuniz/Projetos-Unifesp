#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

sem_t s1, s2;

time_t tempo;

void *t1(void *args)
{
    // while (1)
    {
        time(&tempo);
        if (sem_wait(&s1) != -2){
            if (sem_wait(&s2) != -2){
                printf("t1 %s", ctime(&tempo));
                sem_post(&s2);
                sem_post(&s1);
            }
            // else break;
        }
        // else break;
    }
    pthread_exit(NULL);
}

void *t2(void *args)
{
    // while (1)
    {
        time(&tempo);
        if (sem_wait(&s2) != -2){
            if (sem_wait(&s1) != -2){
                printf("t2 %s", ctime(&tempo));
                sem_post(&s1);
                sem_post(&s2);
            }
            // else break;
        }
        // else break;
    }
    pthread_exit(NULL);
}

int main()
{
    time(&tempo);

    sem_init(&s1, 0, 1);
    sem_init(&s2, 0, 1);

    sem_wait(&s1);
    sem_post(&s1);

    // pthread_t th1, th2;

    // pthread_create(&th1, NULL, t1, NULL);
    // pthread_create(&th2, NULL, t2, NULL);


    // pthread_join(th1, NULL);
    // pthread_join(th2, NULL);
}
