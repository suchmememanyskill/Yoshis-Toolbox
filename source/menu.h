#pragma once

#define MAX_ENTRIES 16

#define BUTTON_A            0xE0E0
#define BUTTON_B            0xE0E1
#define BUTTON_X            0xE0E2
#define BUTTON_Y            0xE0E3
#define BUTTON_L            0xE0E4
#define BUTTON_R            0xE0E5
#define BUTTON_ZL           0xE0E6
#define BUTTON_ZR           0xE0E7
#define BUTTON_SL           0xE0E8
#define BUTTON_SR           0xE0E9
#define BUTTON_UP           0xE0EB
#define BUTTON_DOWN         0xE0EC
#define BUTTON_LEFT         0xE0ED
#define BUTTON_RIGHT        0xE0EE
#define BUTTON_PLUS         0xE0EF
#define BUTTON_MINUS        0xE0F0

typedef struct _menu_item {
    char *name;
    short property;
} menu_item;

typedef struct _menu {
    char name[50];
    menu_item *items;
} menu;

typedef struct _notification {
    char text[75];
    int TTL;
    SDL_Color text_color;
} notification;

void MakeBasicMenu();
void MakeNotification(char *text, int TTL, SDL_Color text_color);
int GetCurrentMenu();
menu_item GetCurrentElement();
void EditTopMenu(menu menu_edit, int pos);
void AddTopMenu(menu menu_object);
void InitTopMenu();