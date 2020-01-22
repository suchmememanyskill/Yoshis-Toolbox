#pragma once
#include "sdl_helper.h"
#include "menu.h"

#define BUFSIZE 32768

char* addstrings(const char *s1, const char *s2);
char* keyboard(char* message, size_t size);
bool checkfolder(char* foldloc);
int copy(const char* src, const char* dst);
int FolderAmount(char *path, char *filter);
menu_item MakeMenuItem(char *text, int property);

enum menuorder {
    MENU_MENU1 = 0,
    MENU_MENU2,
    MENU_PAYLOAD
};