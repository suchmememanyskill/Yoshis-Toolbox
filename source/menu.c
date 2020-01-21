#include "sdl_helper.h"
#include "menu.h"
#include <string.h>
#include <switch.h>

extern int SignalThread();

menu menu_objects[10];
int topmenuamount = 0, currentmenu = 0, selection = 0, offset = 0;
void InitTopMenu(){
    topmenuamount = 0, currentmenu = 0;
}

void AddTopMenu(menu menu_object){
    menu_objects[topmenuamount] = menu_object;
    topmenuamount++;
}

void EditTopMenu(menu menu_edit, int pos){
    menu_objects[pos] = menu_edit;
}

void DrawTopMenu(){
    int xpos = 150, w, h;
    for (int i = 0; i < topmenuamount; i++){
        DrawText(xpos, 28, menu_objects[i].name, COLOR_WHITE, 1920);
        GetTextSize(menu_objects[i].name, &w, &h);

        if (i == currentmenu)
            DrawBox(xpos - 20, 80, w + 40, 13, COLOR_LIGHTBLUE);

        xpos += w + 50;
    }
}

bool ChangeTopMenu(bool Up){
    if (Up){
        if (topmenuamount - 1 > currentmenu){
            currentmenu++;
            return true;
        }
    }
    else {
        if (currentmenu > 0){
            currentmenu--;
            return true;
        }   
    }

    return false;
}

void ClearScreenWithElements(){
    ClearScreen(COLOR_BACKGROUND);
    DrawBox(0, 0, 1919, 93, COLOR_TOP);
    DrawGlyph(50, 20, BUTTON_L, COLOR_WHITE);
    DrawGlyph(1870 - GetButtonTextSize(BUTTON_R), 20, BUTTON_R, COLOR_WHITE);
}

menu_item GetCurrentElement(){
    return menu_objects[currentmenu].items[selection - 1];
}

int GetCurrentMenu(){
    return currentmenu;
}

notification note = {"", 0, COLOR_RED};
void DrawNotification(){
    if (note.TTL > 0){
        note.TTL--;
        int x, y, w, h;

        GetTextSize(note.text, &w, &h);
        x = (1920 - w) / 2;
        y = (80 - h) / 2;
        DrawBox(0, 1000, 1919, 79, COLOR_HIGHLIGHT);
        DrawText(x, 1000 + y, note.text, note.text_color, 1920);
    }
}

void MakeNotification(char *text, int TTL, SDL_Color text_color){
    strcpy(note.text, text);
    note.TTL = TTL;
    note.text_color = text_color;
}

char *shortenstring(char *string){
    static char ret[91] = "";
    if (strlen(string) > 90){
        strlcpy(ret, string, 87);
        for (int i = 86; i < 89; i++)
            ret[i] = '.';
        ret[90] = '\0';
    }
    else 
        strcpy(ret, string);
    
    return ret;
}

void DrawMenuEntry(menu_item item, int x, int y, bool highlighted){
    if (highlighted){
        int w, h;
        GetTextSize(item.name, &w, &h);

        DrawBox(0, y - 5, 1919, 10 + h, COLOR_HIGHLIGHT);
    }

    DrawText(x, y, shortenstring(item.name), COLOR_WHITE, 1920);
}

int GetArrayAmount(int menu_entry){
    int i = 0;

    while (menu_objects[menu_entry].items[i].property != -1)
        i++;
    
    return i;
}

void MakeBasicMenu(){
    int ypos = 150, amount = 0;
    amount = GetArrayAmount(currentmenu);

    while (1){
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);
        ClearScreenWithElements();

        if ((kDown & KEY_LSTICK_UP || kHeld & KEY_RSTICK_UP || kDown & KEY_DUP))
            selection--;
        if ((kDown & KEY_LSTICK_DOWN || kHeld & KEY_RSTICK_DOWN || kDown & KEY_DDOWN))
            selection++;

        if (selection + offset > amount)
            selection = amount - offset;

        else if (selection > MAX_ENTRIES)
            selection = MAX_ENTRIES, offset++;

        if (selection < 1 && offset > 0)
            selection = 1, offset--;

        else if (selection < 1 && offset <= 0)
            selection = 1;

        if (kDown & KEY_L)
            if (ChangeTopMenu(false))
                amount = GetArrayAmount(currentmenu);

        if (kDown & KEY_R)
            if (ChangeTopMenu(true))
                amount = GetArrayAmount(currentmenu);

        if (kDown & KEY_PLUS)
            break;

        if (kDown & KEY_A)
            SignalThread();
        
        ypos = 150;
        for (int i = 0; i < ((amount > MAX_ENTRIES) ? MAX_ENTRIES : amount); i++){
            DrawMenuEntry(menu_objects[currentmenu].items[i + offset], 90, ypos, (i == selection - 1));
            ypos += 50;
        }
        
        DrawTopMenu();
        DrawNotification();
        RenderFrame();
    }
}
