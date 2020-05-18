#include <switch.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "config.h"
#include "utils.h"
#include "ini.h"
#include "listhelper.h"
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

ini_list *atmosphere_config_list[2];
ini_list *atmosphere_config_template[2];
ini_list *temp_ini = NULL;

void get_atmosphere_config_list(ini_list **config1, ini_list **config2){
    *config1 = atmosphere_config_list[0];
    *config2 = atmosphere_config_list[1];
}

void get_atmosphere_template_list(ini_list **config1, ini_list **config2){
    *config1 = atmosphere_config_template[0];
    *config2 = atmosphere_config_template[1];
}

int atmosphere_config_parse(const char* section, const char* name, const char* value){
    ini_list *temp_list;

    if (temp_ini == NULL){
        temp_ini = inilistcreate(section);
    }
    /*
    else if (!strcmp(lastsection, section)){
        inilistadd(section, temp_ini);
        test++;
    }
    */

   for (temp_list = temp_ini; temp_list != NULL; temp_list = temp_list->next){
        if (!strcmp(temp_list->name, section)){
            break;
        }
   }

   if (temp_list == NULL){
       inilistadd(section, temp_ini);
   }
    

    dictlistaddtolastini(name, value, temp_ini);

    /*
    if (temp_ini->firstlistitem == NULL)
        temp_ini->firstlistitem = dictlistcreate(name, value);
    else
        dictlistadd(name, value, temp_ini->firstlistitem);
    */

    return 0;
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

int ReadConfig(char *owninipath){
    ini_parse(owninipath, payload_config_parse);

    ini_parse("/atmosphere/config/system_settings.ini", atmosphere_config_parse);
    atmosphere_config_list[0] = temp_ini;

    temp_ini = NULL;
    ini_parse("/atmosphere/config/override_config.ini", atmosphere_config_parse);
    atmosphere_config_list[1] = temp_ini;

    romfsInit();

    temp_ini = NULL;
    ini_parse("romfs:/system_settings.ini", atmosphere_config_parse);
    atmosphere_config_template[0] = temp_ini;

    temp_ini = NULL;
    ini_parse("romfs:/override_config.ini", atmosphere_config_parse);
    atmosphere_config_template[1] = temp_ini;

    romfsExit();
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
