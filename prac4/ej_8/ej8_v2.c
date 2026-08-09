// compilar : mpicc file.c -o binarry.out
// ejecutar :mpirun -np [number of processes] ./binary_file
// -- con la bandera -np puedo indicar la cantidad de cores que quiero que corran
//  mpirun --use-hwthread-cpus -np 4 ./ej8.out

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char  *argv[])
{
    int num_procs, my_id;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    if (argc < 2)
    {
        if (my_id == 0)
            printf("Usage: %s <length of array>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    int len = atoi(argv[1]);
    int *sendcounts = malloc(sizeof(int) * num_procs) ; // cada posicion del array va a contener la cant de elementos que recibe cada proceso
    int *displs = malloc(sizeof(int) * num_procs) ;  // va a contener los desplazamientos de cada posicion

    int cant_ele  = len / num_procs; // cant elementos de base que va a recibir cada proceso
    int extra = len % num_procs ; // con esta variable vamos manejando si tenemos que poner un offset
 
    int offset = 0 ; 

    for(int i = 0 ; i < num_procs ; i++){
        sendcounts[i] = cant_ele;
        sendcounts[i] += i < extra ? 1 : 0 ;
         
        displs[i] = offset;  // guarda donde empieza este proceso
        offset += sendcounts[i]; // mueve el offset para el sigueinte proceso 
    }
    
    int *arr = NULL;
    if (my_id == 0) // solo el proceso master va a crear el arreglo y llenarlo con los valores del 1 al len, luego va a repartir esos valores entre los procesos, incluido el mismo
    {
        arr = malloc(sizeof(int) *len);
        for (int i = 0; i < len; i++)
            arr[i] = i + 1;
    }
    
    int local_len = sendcounts[my_id]; 
    int *local_arr = malloc(sizeof(int) * local_len);
    // MPI_Scatter(arr, cant_ele, MPI_INT, local_arr, cant_ele, MPI_INT, 0, MPI_COMM_WORLD); // el proceso 0 va a enviar un valor del arreglo a cada proceso, incluido el mismo
    
    MPI_Scatterv(arr , sendcounts , displs, MPI_INT , local_arr ,local_len,MPI_INT  ,0,MPI_COMM_WORLD);
    int local_sum = 0;
    for (int i = 0; i < local_len; i++)
    {
        local_sum += local_arr[i]; // cada proceso va a sumar los valores que le toco, es decir, el valor que recibio mas los valores que le toco a los demas procesos
    }
   
    int reduction_result = 0;
    MPI_Reduce(&local_sum, &reduction_result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); // el proceso 0 va a recibir la suma de los ID de todos los procesos
    if (my_id == 0)
    {
        printf("La suma de los ID de todos los procesos es: %d\n", reduction_result);
        free(arr);
    }
    free(local_arr);
    MPI_Finalize();    
    return 0;
}
 