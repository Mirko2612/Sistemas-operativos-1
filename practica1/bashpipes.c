#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#define NMAX_ENTRADAS 256
#define NMAX_COMANDOS 128

int main(){
    char* args[NMAX_ENTRADAS];
    char* arr_comandos [NMAX_COMANDOS];
    int numCmd;
    char entrada[NMAX_ENTRADAS];
    
    while (1)
    {
        printf("BashCasaCasera:$ ");
        
        fgets(entrada, sizeof(entrada), stdin); // leemos la entrada
        entrada[strcspn(entrada, "\n")] = '\0'; 

        char* comando = strtok(entrada, "|"); 
        while (comando != NULL && numCmd < NMAX_COMANDOS){ //tokenizamos los comandos
            arr_comandos[numCmd++] = comando;
            comando = strtok(NULL, "|");
        }
        int prev_fd = -1; // fd previo, indica si es el primero en -1
        for (int j = 0; j < numCmd; j++)
        {
            int pipefd[2]; // declaramos el pipe
            if(j < numCmd -1)
            { //si no somos el ultimo proceso
                if(pipe(pipefd) < 0){ //si falla el pipe
                    perror("pipe");
                    exit(1);
                }
            }
            
            pid_t pid = fork();

            if (pid == 0) // ------HIJO--------
            {
                int i=0;
                int fd = -1;
                if (prev_fd != -1) //si no somos el primer pipe
                {
                    dup2(prev_fd, STDIN_FILENO); // reedirijo la entrada
                    close(prev_fd);
                }
                if (j < numCmd -1) //si no somos el ultimo pipe
                {
                    dup2(pipefd[1],STDOUT_FILENO); //reedirijo la salida
                    close(pipefd[0]);
                    close(pipefd[1]);
                }
                // PROCESAMIENTO DE ARGUMENTOS DEL COMANDO ACTUAL
                char* token = strtok(arr_comandos[j]," ");
        
                while (token != NULL && i < 256) //tokenizamos los args
                {
                    args[i++] = token;
                    token = strtok(NULL, " ");
                }
                
                if (i > 2 && strcmp(args[i - 2], ">") == 0){ //EN CASO DE ENCONTRAR REDIRECCION
                    fd = open(args[i-1], O_CREAT | O_WRONLY | O_TRUNC, 0644); //abrimos el archivo de salida
                    dup2(fd,STDOUT_FILENO); //reedirijo la salida
                    close(fd);
                    
                    if (fd < 0) { //si falla el fd
                        perror("open");
                        exit(1);
                    }
                    
                    args[i-2] = NULL; //anulamos el primer argumento no utilizado
                }

                args[i] = NULL; //anulamos la posicion no utilizada
                
                execvp(args[0], args); //ejecutamos el comando
                
                perror("execvp");
                exit(1);
            }
            // ------- PADRE ----------
            if (prev_fd != -1)
                close(prev_fd); // cerramos el fd previo si ya ocurrio el primer pipe
            
            if (j < numCmd -1) // si nuestro hijo no es el ultimo
            {
                close(pipefd[1]); // cerrmos el fd de escritura
                prev_fd = pipefd[0]; // el nuevo fd previo va a ser mi lectura actual
            }
            
        }
        // ESPERAMOS A QUE TODOS LOS HIJOS TERMINEN
        for (int j = 0; j < numCmd; j++)
            wait(NULL);
        
    }
    return 0;
}