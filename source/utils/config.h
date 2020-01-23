#pragma once

typedef struct {
    char *path;
    char *fav;
} payload_config;

int ReadPayloadConfig(char *path);
void SetPayloadFolder(char *in);
void SetPayloadFav(char *in);
void WriteConfig();
payload_config get_payload_config();