#pragma once

typedef struct _dict {
    char *key;
    char *value;
    struct _dict *next;
} dict; 

typedef struct _ini_list {
    char *name;
    dict *firstlistitem;
    struct _ini_list *next;
} ini_list;

ini_list *inilistcreate(char *name);
void inilistadd(char *name, ini_list *list);
dict *dictlistcreate(char *key, char *value);
void dictlistadd(char *key, char *value, dict *list);
void dictlistaddtolastini(char *key, char *value, ini_list *list);
int dictgetlistamount(dict *list);
int inigetdictlistamount(ini_list *list);