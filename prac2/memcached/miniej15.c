#include "ej15.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

char *strdup(const char *s) {
    char *p = malloc(strlen(s) + 1);
    if (p) strcpy(p, s);
    return p;
}

Item* create_item(char* key, char* value)
{
    Item* Nitem = malloc(sizeof(Item));
    assert(Nitem);

    Nitem->eliminated = 0;
    Nitem->key = strdup(key);
    Nitem->value = strdup(value);

    return Nitem;
}

int is_key_exists(char *key, Item *store[], int max_items)
{
    for (int i = 0; i < max_items; i++)
    {
        if (store[i] != NULL && !store[i]->eliminated && strcmp(store[i]->key, key) == 0)
            return 1; // La clave existe
    }
    return 0; // La clave no existe
}

void put_item(Item *item, Item *store[], int max_items)
{
    // Implementar la función para guardar el item en el store
    for (int i = 0; i < max_items; i++)
    {
        if (store[i] == NULL || store[i]->eliminated)
        {
            store[i] = item;
            return;
        }
    }
}

Item *get_item(char *key, Item *store[], int max_items)
{
    // Implementar la función para buscar el item en el store
    for (int i = 0; i < max_items; i++)
    {
        if (store[i] != NULL && !store[i]->eliminated && strcmp(store[i]->key, key) == 0)
            return store[i];
    }
    return NULL;
}

void del_item(char *key, Item *store[], int max_items)
{
    // Implementar la función para eliminar el item del store
    for (int i = 0; i < max_items; i++)
    {
        if (store[i] != NULL && !store[i]->eliminated && strcmp(store[i]->key, key) == 0)
        {
            free(store[i]->key);
            free(store[i]->value);
            store[i]->eliminated = 1;
            return;
        }
    }
}

void del_store(Item* store[], int max_items)
{
    for (int i = 0; i < max_items; i++)
    {
        if(store[i] != NULL)
            free(store[i]);
    }
    
}