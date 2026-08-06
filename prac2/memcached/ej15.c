#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "ej15.h"
#define KEYP "PUT"
#define KEYD "DEL"
#define KEYG "GET"
#define MAXITEMS 256

Item* store[256];

pthread_mutex_t mutex_store = PTHREAD_MUTEX_INITIALIZER;

int fd_readline(int fd, char *buf)
{
	int rc;
	int i = 0;

	/*
	 * Leemos de a un caracter (no muy eficiente...) hasta
	 * completar una línea.
	 */
	while ((rc = read(fd, buf + i, 1)) > 0) {
		if (buf[i] == '\n')
			break;
		i++;
	}

	if (rc < 0)
		return rc;

	buf[i] = 0;
	return i;
}


void quit(char* coderr)
{
    perror(coderr);
    abort();
}

void* handle_conn(void* arg)
{
    int csock = *(int*)arg;
    char buff[256];
    int rc;
    char* saveptr;
    while (1)
    {
        /* Atendemos pedidos, uno por linea */
		rc = fd_readline(csock, buff);
        if (rc < 0)
        {
            free((int*)arg);
            quit("read");
        }
        if (rc == 0)
        {
            /*Linea vacia se cerro la conexion*/
            free((int*)arg);
            close(csock);
            return NULL;
        }
        /*Parseo y ejecucion de comandos*/
        char* cmd = strtok_r(buff," ",&saveptr);
        if (strcmp(cmd, KEYP) == 0) // si put k v
        {
            char* key = strtok_r(NULL, " ", &saveptr);

            char* value = strtok_r(NULL, " ", &saveptr);

            Item* item = create_item(key, value);

            /*seccion critica*/
            pthread_mutex_lock(&mutex_store);
            put_item(item, store, MAXITEMS);
            pthread_mutex_unlock(&mutex_store);
            write(csock, "OK\n",3);
        }
        else if(strcmp(cmd, KEYG) == 0) //si get k
        {
            char* key = strtok_r(NULL, " ", &saveptr);

            /*seccion critica*/
            pthread_mutex_lock(&mutex_store);
            Item* item = get_item(key, store, MAXITEMS);
            pthread_mutex_unlock(&mutex_store);
            /*end of sc*/
            if (item == NULL)
                write(csock, "NOTFOUND\n", 10);
            else
            {
                write(csock, "OK ",3);
                write(csock, item->value, strlen(item->value));
                write(csock, "\n", 1);
            }
        }
        else if (strcmp(cmd, KEYD) == 0)
        {
            char* key = strtok_r(NULL, " ", &saveptr);
            
            pthread_mutex_lock(&mutex_store);
            del_item(key, store, MAXITEMS);
            pthread_mutex_unlock(&mutex_store);
            write(csock, "OK\n", 3);
        }
        else
        {
            write(csock, "EINVAL\n", 7);
            free((int*)arg);
            return NULL;
        }
    }
    
    return NULL;
}

void wait_for_clients(int lsock)
{
    int csock;

    csock = accept(lsock, NULL, NULL); // esperamos conexion no nos interesa quien.

    if (csock < 0)
        quit("accept");
    
    /* atendemos al cliente*/
    int* csock_ptr = malloc(sizeof(int));
    *csock_ptr = csock;
    /* hilo */
    pthread_t th;
    int status = pthread_create(&th, NULL, handle_conn, (void*)csock_ptr);

    if (status < 0){
        free(csock_ptr);
        quit("pthread");
    }
    
    pthread_detach(th); // Desasociamos el hilo para que libere sus recursos al terminar
    /* hilo principal sigue aceptando clientes*/
    wait_for_clients(lsock);
}

int mk_lsock()
{
    struct sockaddr_in sa;
    int lsock;
    int rc;
    int yes = 1;
    lsock = socket(AF_INET, SOCK_STREAM, 0); // creo socket y obtengo fd

    if (lsock < 0)
        quit("socket");

    /* Setear opción reuseaddr... normalmente no es necesario */
	if (setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == 1)
		quit("setsockopt");

    sa.sin_family = AF_INET;
    sa.sin_port = htons(3942);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    rc = bind(lsock, (struct sockaddr*)&sa, sizeof(sa)); // bindeamos al puerto 3942 TCP
    if (rc < 0)
       quit("bind");
    
    rc = listen(lsock, 10); //setear en modo escucha
    
    if (rc < 0)
        quit("listen");
    
    return lsock;
}


int main(){
    int lsock;
    lsock = mk_lsock(); //creacion del socket

    wait_for_clients(lsock); // escucha del servidor

    del_store(store, MAXITEMS); //eliminado de la store
    
    return 0;
}
