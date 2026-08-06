#ifndef EJ15_H
#define EJ15_H
#include <stdlib.h>

typedef struct
{
    char *key;
    char *value;
    int eliminated;
} Item;

// enum
// {
//     PUT,
//     GET,
//     DEL,
// } Command;

// enum
// {
//     OK,
//     NOT_FOUND,
//     EINVAL,
// } Response;
Item* create_item(char* key, char* value);
void put_item(Item *item, Item *store[], int max_items);
Item *get_item(char *key, Item *store[], int max_items);
void del_item(char *key, Item *store[], int max_items);
int is_key_exists(char *key, Item *store[], int max_items);
void del_store(Item* store[], int max_items);
#endif // EJ15_H