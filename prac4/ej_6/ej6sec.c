#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#define TAM 50000000
void swap(int *a, int *b)
{
    int aux = *a;
    *a = *b;
    *b = aux;
}

/* Particion de Lomuto, tomando el primer elemento como pivote */
int particionar(int a[], int N)
{
    int i, j = 0;
    int p = a[0];

    swap(&a[0], &a[N-1]);

    for (i = 0; i < N-1; i++) {
        if (a[i] <= p)
            swap(&a[i], &a[j++]);
    }

    swap(&a[j], &a[N-1]);

    return j;
}

void qsortt(int a[], int N)
{
    if (N < 2)
        return;

    int p = particionar(a, N);

    qsortt(a, p);
    qsortt(a + p + 1, N - p - 1);
}

void imprimir(int a[], int N)
{
    for (int i = 0; i < N; i++)
        printf("%d ", a[i]);

    printf("\n");
}



int main(void)
{
    srand(time(NULL));
    int* a = malloc(sizeof(int) * TAM);
    assert(a);
    for (int i = 0; i < TAM; i++)
        a[i] = rand() % TAM;
    

    printf("Antes:\n");
    imprimir(a, TAM);

    qsortt(a, TAM);

    printf("Despues:\n");
    imprimir(a, TAM);
    free(a);
    return 0;
}