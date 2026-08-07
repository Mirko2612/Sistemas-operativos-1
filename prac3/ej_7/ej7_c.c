#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

/*
    Problema de los fumadores
    Solución con pushers
    SIN modificar el código del agente
*/

sem_t tabaco;
sem_t papel;
sem_t fosforos;

sem_t otra_vez;

sem_t turno1; // fumador con fosforos
sem_t turno2; // fumador con papel
sem_t turno3; // fumador con tabaco

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/* estado interno protegido por mutex */
int hay_tabaco = 0;
int hay_papel = 0;
int hay_fosforos = 0;


/* ===================================================== */
/* AGENTE (NO MODIFICADO)                                */
/* ===================================================== */

void agente()
{
    while (1)
    {
        sem_wait(&otra_vez);

        int caso = random() % 3;

        if (caso != 0)
            sem_post(&fosforos);

        if (caso != 1)
            sem_post(&papel);

        if (caso != 2)
            sem_post(&tabaco);
    }
}


/* ===================================================== */
/* PUSHERS                                               */
/* ===================================================== */

/*
    Cuando llega TABACO:
    - si ya había PAPEL -> fuma el que tiene fosforos
    - si ya había FOSFOROS -> fuma el que tiene papel
    - sino guardamos tabaco
*/
void* pusher_tabaco(void* arg)
{
    while (1)
    {
        sem_wait(&tabaco);

        pthread_mutex_lock(&mutex);

        if (hay_papel)
        {
            hay_papel = 0;

            sem_post(&turno1);
        }
        else if (hay_fosforos)
        {
            hay_fosforos = 0;

            sem_post(&turno2);
        }
        else
        {
            hay_tabaco = 1;
        }

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}


/*
    Cuando llega PAPEL:
    - si ya había TABACO -> fuma el que tiene fosforos
    - si ya había FOSFOROS -> fuma el que tiene tabaco
*/
void* pusher_papel(void* arg)
{
    while (1)
    {
        sem_wait(&papel);

        pthread_mutex_lock(&mutex);

        if (hay_tabaco)
        {
            hay_tabaco = 0;

            sem_post(&turno1);
        }
        else if (hay_fosforos)
        {
            hay_fosforos = 0;

            sem_post(&turno3);
        }
        else
        {
            hay_papel = 1;
        }

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}


/*
    Cuando llega FOSFOROS:
    - si ya había TABACO -> fuma el que tiene papel
    - si ya había PAPEL -> fuma el que tiene tabaco
*/
void* pusher_fosforos(void* arg)
{
    while (1)
    {
        sem_wait(&fosforos);

        pthread_mutex_lock(&mutex);

        if (hay_tabaco)
        {
            hay_tabaco = 0;

            sem_post(&turno2);
        }
        else if (hay_papel)
        {
            hay_papel = 0;

            sem_post(&turno3);
        }
        else
        {
            hay_fosforos = 1;
        }

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}


/* ===================================================== */
/* FUMADORES                                             */
/* ===================================================== */

void fumar(int fumador)
{
    printf("Fumador %d: Puf! Puf! Puf!\n", fumador);

    sleep(1);
}


/*
    fumador1 tiene fosforos
    necesita tabaco + papel
*/
void* fumador1(void* arg)
{
    while (1)
    {
        sem_wait(&turno1);

        fumar(1);

        sem_post(&otra_vez);
    }

    return NULL;
}


/*
    fumador2 tiene papel
    necesita tabaco + fosforos
*/
void* fumador2(void* arg)
{
    while (1)
    {
        sem_wait(&turno2);

        fumar(2);

        sem_post(&otra_vez);
    }

    return NULL;
}


/*
    fumador3 tiene tabaco
    necesita papel + fosforos
*/
void* fumador3(void* arg)
{
    while (1)
    {
        sem_wait(&turno3);

        fumar(3);

        sem_post(&otra_vez);
    }

    return NULL;
}


/* ===================================================== */
/* MAIN                                                  */
/* ===================================================== */

int main()
{
    pthread_t f1, f2, f3;

    pthread_t pt, pp, pf;

    sem_init(&tabaco, 0, 0);
    sem_init(&papel, 0, 0);
    sem_init(&fosforos, 0, 0);

    sem_init(&otra_vez, 0, 1);

    sem_init(&turno1, 0, 0);
    sem_init(&turno2, 0, 0);
    sem_init(&turno3, 0, 0);


    pthread_create(&f1, NULL, fumador1, NULL);
    pthread_create(&f2, NULL, fumador2, NULL);
    pthread_create(&f3, NULL, fumador3, NULL);

    pthread_create(&pt, NULL, pusher_tabaco, NULL);
    pthread_create(&pp, NULL, pusher_papel, NULL);
    pthread_create(&pf, NULL, pusher_fosforos, NULL);


    agente();

    return 0;
}