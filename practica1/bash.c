#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
int main(){
    char entrada[256];
    char* args[256];
    int i=0;
    int fd = -1;
    while (1)
    {
        printf("BashCasaCasera:$ ");
        
        fgets(entrada, sizeof(entrada), stdin);
        entrada[strcspn(entrada, "\n")] = '\0';
        char* token = strtok(entrada," ");
        
        while (token != NULL && i < 256)
        {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        
        if (i > 2 && strcmp(args[i - 2], ">") == 0){
            fd = open(args[i-1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (fd < 0) {
                perror("open");
                exit(1);
            }
            args[i-2] = NULL;
        }
        
        args[i] = NULL;

        pid_t pid = fork();
        
        if(pid == 0){
            if(fd != -1){
                dup2(fd,STDOUT_FILENO);
                close(fd);
            }
            execvp(args[0], args);
            exit(1);
        }
        else
        wait(NULL);

        i=0;
        fd = -1;
    }
    return 0;
}