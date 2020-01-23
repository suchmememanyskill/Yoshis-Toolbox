#include <switch.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "utils.h"
#include "../menu/sdl_helper.h"
#include "../menu/menu.h"
#include "ini.h"

int copy(const char* src, const char* dst){
    FILE* in = fopen(src, "rb");
    FILE* out = fopen(dst, "wb");
    if(in == NULL)
        return -1;
	if(out == NULL)
		return -2;
    else
    {
        size_t len = 0;
        char buffer[BUFSIZE];
        while((len = fread(buffer, 1, BUFSIZE, in)) > 0)
            fwrite(buffer, 1, len, out);
    }
    if(in)
        fclose(in);
    if(out)
        fclose(out);

	return 0;
}

bool checkfolder(char* foldloc){
	DIR *tr = opendir(foldloc);
	bool folderexists = false;
	if (tr != NULL){
			folderexists = true;
    	}
    closedir(tr);
    return folderexists;
}

char* keyboard(char* message, size_t size){
	SwkbdConfig	skp; 
	Result keyrc = swkbdCreate(&skp, 0);
	char* out = NULL;
	out = (char *)calloc(sizeof(char), size + 1);

	if (R_SUCCEEDED(keyrc) && out != NULL){
		swkbdConfigMakePresetDefault(&skp);
		swkbdConfigSetGuideText(&skp, message);
		keyrc = swkbdShow(&skp, out, size);
		swkbdClose(&skp);	
	}

	else {
	free(out);
	out = NULL;
	}

	return (out);
}

char* addstrings(const char *s1, const char *s2){
    static char *result;
    if (result != NULL){
        free(result);
        result = NULL;
    }
    result = malloc(strlen(s1) + strlen(s2) + 1); 
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

int FolderAmount(char *path, char *filter){
    struct dirent *de;
    DIR *dr = opendir(path);
    int amount = 0;
    bool checkwithfilter = (strlen(filter) > 0);

    while ((de = readdir(dr)) != NULL)
        if (checkwithfilter){
            if (strstr(de->d_name, ".bin") != NULL)
                amount++;
        }
        else
            amount++;

    closedir(dr);

    if (amount == 0)
        amount = 1;

    return amount;
}

menu_item MakeMenuItem(char *text, int property){
    menu_item item;
    item.property = property;
    item.name = makestring(text);
    return item;
}

char *makestring(const char *in){
    char *ret;
    ret = malloc(strlen(in) + 1);
    strcpy(ret, in);
    return ret;
}