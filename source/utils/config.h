#pragma once
#include "listhelper.h"

typedef struct {
    char *path;
    char *fav;
} payload_config;

int ReadPayloadConfig(char *path);
void SetPayloadFolder(char *in);
void SetPayloadFav(char *in);
void WriteConfig();
payload_config get_payload_config();
int ReadConfig(char *owninipath);
void get_atmosphere_config_list(ini_list **config1, ini_list **config2);