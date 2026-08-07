#include <stdlib.h>
#include <assert.h>
#include "barrier.h"

// return a pointer of a barrier struct
barrier* barrier_create()
{
    barrier* barrera = malloc(sizeof(barrier)); // asigning virtual memory for barrier
    
    assert(barrera);
    
    return barrera;
}

// initialize the barrier of the pointer to a barrier struct b,for n threads
void barrier_init(barrier *b, int n)
{
    pthread_mutex_init(&(b->mutex), NULL);
    pthread_cond_init(&(b->cond), NULL);
    b->count = 0; 
    b->total = n;
    b->generation = 0;
}

// wait n nthreads of b before proceding
void barrier_wait(barrier *b)
{
  pthread_mutex_lock(&(b->mutex)); // take the mutex for next check
  
  b->count ++;  
  
  int gen = b->generation;
  
  if (b->count == b->total) // if count of threads is equal as generation of barrier 
  {
    b->generation++;
    
    b->count = 0;
    
    pthread_cond_broadcast(&(b->cond));
  }
  else
  {
    while (gen == b->generation) // Mesa logic
        pthread_cond_wait(&(b->cond), &(b->mutex)); // threads wait the signal

  }
  pthread_mutex_unlock(&(b->mutex)); // unlock the mutex
}

// destroy the barrier
void barrier_destroy(barrier *b)
{
    pthread_mutex_destroy(&b->mutex);
    pthread_cond_destroy(&b->cond);
    free(b);
}