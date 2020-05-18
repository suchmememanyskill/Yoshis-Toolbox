#include <stdlib.h>
#include <stdio.h>
#include <switch.h>
#include <dirent.h> 
#include <sys/stat.h>
#include <string.h>
#include "ams_config.h"
#include "../menu/sdl_helper.h"
#include "../menu/menu.h"
#include "../utils/utils.h"
#include "../utils/config.h"

menu_item *ams_configs;
ini_list *atmosphere_config_list[2];
ini_list *atmosphere_config_template[2];

void SetMenuAmsConfig(){
    EditTopMenu((menu) {"AMS_Config", ams_configs, KEY_A}, MENU_AMS_CONFIG);
}

void ClearAmsConfigItems(){
    int i = 0;

    if (ams_configs != NULL){
        while (ams_configs[i].name != NULL){
            free(ams_configs[i].name);
            ams_configs[i].name = NULL;
            i++;
        }
        free(ams_configs);
        ams_configs = NULL;
    }
}

void CreateAmsConfigItems(int amount){
    ClearAmsConfigItems();
    ams_configs = calloc(amount + 1, sizeof(menu_item));
    ams_configs[amount] = (menu_item) {NULL, -1};   
}

void SetAmsConfigList(){
    get_atmosphere_config_list(&atmosphere_config_list[0], &atmosphere_config_list[1]);
    get_atmosphere_template_list(&atmosphere_config_template[0], &atmosphere_config_template[1]);
}

void EntryHandler(dict *entry, int number, int arraynumb){
    dict *res = Search(entry->key, atmosphere_config_template[arraynumb]);
    char *temp_char;

    if (res != NULL){
        if (!strcmp(res->value, "u8!")){
            temp_char = malloc(strlen(entry->key) + 9);
            sprintf(temp_char, "%s = %s", entry->key, ((entry->value[strlen(entry->value) - 1] == '0') ? "True" : "False"));
            ams_configs[number] = MakeMenuItem(temp_char, 1);
            free(temp_char);
            return;
        }

        if (!strcmp(res->value, "bool")){
            temp_char = malloc(strlen(entry->key) + 9);
            sprintf(temp_char, "%s = %s", entry->key, ((!strcmp(entry->value, "true")) ? "True" : "False"));
            ams_configs[number] = MakeMenuItem(temp_char, 1);
            free(temp_char);
            return;
        }

    }

    temp_char = malloc(strlen(entry->key) + strlen(entry->value) + 4);

    sprintf(temp_char, "%s = %s", entry->key, entry->value);
    ams_configs[number] = MakeMenuItem(temp_char, 1);

    free(temp_char);
}

void AddItemsToArray(ini_list *list, int initialpos, int arraynumb){
    ini_list *temp_list = list;
    dict *temp_dict;
    int i = initialpos;
    char *temp_char;

    for (temp_list = list; temp_list != NULL; temp_list = temp_list->next){
        if (temp_list->firstlistitem != NULL){
            temp_char = malloc(strlen(temp_list->name) + 3);

            sprintf(temp_char, "[%s]", temp_list->name);
            ams_configs[i++] = MakeMenuItem(temp_char, 0);

            free(temp_char);

            for (temp_dict = temp_list->firstlistitem; temp_dict != NULL; temp_dict = temp_dict->next){
                EntryHandler(temp_dict, i, arraynumb);
                i++;
            }
        }
    }
}

void BuildAmsConfigMenu(){
    int config1len, config2len;
    config1len = inigetdictlistamount(atmosphere_config_list[0]) + inigetlistamount(atmosphere_config_list[0]);
    config2len = inigetdictlistamount(atmosphere_config_list[1]) + inigetlistamount(atmosphere_config_list[1]);

    CreateAmsConfigItems(config1len + config2len + 3);
    
    ams_configs[0] = MakeMenuItem("-- System Settings --", 0);
    ams_configs[config1len + 1] = MakeMenuItem("", 0);
    ams_configs[config1len + 2] = MakeMenuItem("-- Controller Config --", 0);

    AddItemsToArray(atmosphere_config_list[0], 1, 0);
    AddItemsToArray(atmosphere_config_list[1], config1len + 3, 1);

    /*
    ini_list *temp_list;
    dict *temp_dict;
    temp_list = atmosphere_config_list[0];
    */

    /*
    while (temp_list->next != NULL){
        if (temp_list->firstlistitem != NULL){
            temp_dict = temp_list->firstlistitem;
            while (temp_dict->next != NULL){
                char *temp_char = malloc(strlen(temp_dict->key) + strlen(temp_dict->value) + 7);
                sprintf(temp_char, "%s = %s", temp_dict->key, temp_dict->value);
                ams_configs[i++] = MakeMenuItem(temp_char, 1);
                free(temp_char);
                temp_dict = temp_dict->next;
            }
        }
        temp_list = temp_list->next;
    }
    
    i += 2;

    temp_list = atmosphere_config_list[1];

    while (temp_list->next != NULL){
        if (temp_list->firstlistitem != NULL){
            temp_dict = temp_list->firstlistitem;
            while (temp_dict->next != NULL){
                char *temp_char = malloc(strlen(temp_dict->key) + strlen(temp_dict->value) + 4);
                sprintf(temp_char, "%s = %s", temp_dict->key, temp_dict->value);
                ams_configs[i++] = MakeMenuItem(temp_char, 1);
                free(temp_char);
                temp_dict = temp_dict->next;
            }
        }
        temp_list = temp_list->next;
    }
    */
}

void InitAmsConfigMenu(){
    SetAmsConfigList();
    BuildAmsConfigMenu();
    SetMenuAmsConfig();
}