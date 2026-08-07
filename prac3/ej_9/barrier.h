#ifndef BARRIER_H
#define BARRIER_H
#include <pthread.h>

typedef struct BARRIER
{
    pthread_mutex_t mutex; // mutex that block waiting threads
    pthread_cond_t cond; // conditional variable of barrier 
    int count; // number of threads in the barrier
    int total; // amount of threads to wait
    int generation; // actual generation of barrier
} barrier;

// return a pointer of a barrier struct
barrier* barrier_create();

// initialize the barrier of the pointer to a barrier struct b,for n threads 
void barrier_init(barrier *b, int n);

// wait n nthreads of b before proceding
void barrier_wait(barrier *b);

// destroy the barrier
void barrier_destroy(barrier* b);
#endif // BARRIER_H