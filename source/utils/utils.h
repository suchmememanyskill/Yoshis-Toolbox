#pragma once
#include "../menu/sdl_helper.h"
#include "../menu/menu.h"

#define BUFSIZE 32768
#define INILOC "yoshis-toolbox.ini"

char* addstrings(const char *s1, const char *s2);
char* keyboard(char* message, size_t size);
bool checkfolder(char* foldloc);
int copy(const char* src, const char* dst);
int FolderAmount(char *path, char *filter);
menu_item MakeMenuItem(char *text, int property);
char *makestring(const char *in);

enum menuorder {
    MENU_PAYLOAD = 0,
    MENU_OPTIONS,
    MENU_AMS_CONFIG
};