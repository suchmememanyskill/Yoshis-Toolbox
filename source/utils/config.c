#include <switch.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "config.h"
#include "utils.h"
#include "ini.h"
#include "../tools/payload.h"

payload_config payloadconfig;

payload_config get_payload_config(){
    return payloadconfig;
}

int payload_config_parse(const char* section, const char* name, const char* value){
    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

    if (MATCH("Payload", "Path"))
        payloadconfig.path = makestring(value);
    else if (MATCH("Payload", "Fav"))
        payloadconfig.fav = makestring(value);
    else
        return 0;

    return 1;
}

void WriteConfig(){
    FILE *fp;

    if (payloadconfig.path == NULL)
        payloadconfig.path = makestring("");

    if (payloadconfig.fav == NULL)
        payloadconfig.fav = makestring("");


    fp = fopen(INILOC, "w+");
    fprintf(fp, "[Payload]\nPath=%s\nFav=%s\n", payloadconfig.path, payloadconfig.fav);

    fclose(fp);
}

int ReadPayloadConfig(char *path){
    return ini_parse(path, payload_config_parse);
}

void SetPayloadFolder(char *in){
    if (payloadconfig.path != NULL)
        free(payloadconfig.path);
    
    payloadconfig.path = makestring(in);
    SetPayloadConfig();
}

void SetPayloadFav(char *in){
    if (payloadconfig.fav != NULL)
        free(payloadconfig.fav);
    
    payloadconfig.fav = makestring(in);
    SetPayloadConfig();
}
