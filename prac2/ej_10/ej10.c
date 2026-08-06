#include <stdio.h>
#include <pthread.h>
#define TAMANIO 5
int arreglo[TAMANIO] = {10,20,30,40,50};

int inicio = 0;
    
int mitad = TAMANIO/2;

int final = TAMANIO;

int SumaTotal = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* sumar_arr(void* arg){
    for (int i = inicio; i < mitad; i++)
    {
        pthread_mutex_lock(&mutex);
        SumaTotal = SumaTotal + arreglo[i];
        pthread_mutex_unlock(&mutex); 
    }
    
    return NULL;
}

void* sumar_arr2(void* arg){
    for (int i = mitad; i < final; i++)
    {
        pthread_mutex_lock(&mutex);
        SumaTotal = SumaTotal + arreglo[i];
        pthread_mutex_unlock(&mutex); 
    }
    
    return NULL;
}
int main(){
    pthread_t t1, t2;

    pthread_create(&t1, NULL, sumar_arr, NULL);
    
    pthread_create(&t2, NULL, sumar_arr2, NULL);

    pthread_join(t1, NULL);
    
    pthread_join(t2, NULL);
    
    printf("%d\n", SumaTotal);

    return 0;
}