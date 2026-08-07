#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "VCond3.h"

#define NTHREADS 5

Cond_V cv;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int listo = 0;

void* worker(void* arg)
{
    long id = (long)arg;

    pthread_mutex_lock(&mutex);

    while (!listo)
    {
        printf("Thread %ld esperando\n", id);
        condv_wait(&cv, &mutex);
    }

    printf("Thread %ld despertado\n", id);

    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(void)
{
    pthread_t th[NTHREADS];

    condv_init(&cv);

    for (long i = 0; i < NTHREADS; i++)
        pthread_create(&th[i], NULL, worker, (void*)i);

    sleep(2);

    printf("\n=== SIGNAL ===\n");

    pthread_mutex_lock(&mutex);
    condv_signal(&cv);
    pthread_mutex_unlock(&mutex);

    sleep(2);

    printf("\n=== BROADCAST ===\n");

    pthread_mutex_lock(&mutex);
    listo = 1;
    condv_broadcast(&cv);
    pthread_mutex_unlock(&mutex);

    for (int i = 0; i < NTHREADS; i++)
        pthread_join(th[i], NULL);

    condv_destroy(&cv);

    return 0;
}