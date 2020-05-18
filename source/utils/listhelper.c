#include <switch.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "utils.h"
#include "ini.h"
#include "listhelper.h"

ini_list *inilistcreate(char *name){
    ini_list *ret;
    ret = malloc(sizeof(ini_list));
    ret->name = makestring(name);
    ret->firstlistitem = NULL;
    ret->next = NULL;
    return ret;
}

void inilistadd(char *name, ini_list *list){
    ini_list *add, *temp;
    add = inilistcreate(name);

    temp = list;
    while(temp->next != NULL){
        temp = temp->next;
    }

    temp->next = add;
}

dict *dictlistcreate(char *key, char *value){
    dict *ret;
    ret = malloc(sizeof(dict));
    ret->key = makestring(key);
    ret->value = makestring(value);
    ret->next = NULL;
    return ret;
}

void dictlistadd(char *key, char *value, dict *list){
    dict *add, *temp;
    add = dictlistcreate(key, value);

    temp = list;
    while(temp->next != NULL){
        temp = temp->next;
    }

    temp->next = add;
}

void dictlistaddtolastini(char* key, char* value, ini_list* list) {
    ini_list* temp;

    temp = list;
    while (temp->next != NULL)
        temp = temp->next;

    if (temp->firstlistitem == NULL)
        temp->firstlistitem = dictlistcreate(key, value);
    else {
        dictlistadd(key, value, temp->firstlistitem);
    }
}

int dictgetlistamount(dict *list){
    int i = 0;
    dict *temp;

    for (temp = list; temp != NULL; temp = temp->next)
        i++;

    return i;
}

int inigetdictlistamount(ini_list *list){
    ini_list *temp_list;
    int i = 0;

    if (list == NULL)
        return 0;

    for (temp_list = list; temp_list != NULL; temp_list = temp_list->next) {
        if (temp_list->firstlistitem != NULL)
            i += dictgetlistamount(temp_list->firstlistitem);
    }

    return i;
}

int inigetlistamount(ini_list *list){
    int i = 0;
    ini_list *temp_list;

    for (temp_list = list; temp_list != NULL; temp_list = temp_list->next)
        i++;

    return i;
}

dict *Search(char *searchstring, ini_list *list){
    ini_list *temp_list;
    dict *temp_dict;

    for (temp_list = list; temp_list != NULL; temp_list = temp_list->next){
        for (temp_dict = temp_list->firstlistitem; temp_dict != NULL; temp_dict = temp_dict->next){
            if (!strcmp(searchstring, temp_dict->key))
                return temp_dict;
        }
    }

    return NULL;
}