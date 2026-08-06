#include <stdio.h>
#include <pthread.h>
#include <stdio.h>
#include <stdatomic.h>

#define VISITANTES 40000

int VisitantesRegistrados = 0;

atomic_int mutex = 0; // creo mutex para proteger seccion critica


void* molinete1(void* arg){
    int zero = 0;
    for (int i = 0; i < VISITANTES/2; i++){    
        while(!atomic_compare_exchange_strong(&mutex, &zero, 1)){;} // mientras no pueda acceder al mutex espero
        VisitantesRegistrados++;
        fputc('X',stdout);
        atomic_store(&mutex, 0);
    }
        return NULL;
}

void* molinete2(void* arg){
    int zero = 0;
    for (int i = 0; i < VISITANTES/2; i++){  
        while(!atomic_compare_exchange_strong(&mutex, &zero, 1)){;}
        VisitantesRegistrados++;
        fputc('O',stdout);
        atomic_store(&mutex, 0);
    }
        return NULL;
}

int main(){
    pthread_t t1, t2;

    pthread_create(&t1, NULL, molinete1, NULL);
    
    pthread_create(&t2, NULL, molinete2, NULL);  

    pthread_join(t1, NULL);
    
    pthread_join(t2, NULL);        

    printf("Visitantes que se registraron %d\n",VisitantesRegistrados);

    return 0;
}