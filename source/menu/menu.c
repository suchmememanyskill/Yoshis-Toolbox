#include "sdl_helper.h"
#include "menu.h"
#include "../utils/utils.h"
#include <string.h>
#include <switch.h>
#include "../tools/payload.h"
#include <time.h>

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
    return menu_objects[currentmenu].items[selection + offset - 1];
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
    if (!strcmp(item.name, ""))
        return;

    if (highlighted){
        int w, h;
        GetTextSize(item.name, &w, &h);

        DrawBox(0, y - 5, 1919, 10 + h, COLOR_HIGHLIGHT);
    }

    if (item.property == 0)
        DrawText(x, y, shortenstring(item.name), COLOR_LIGHTGREY, 1920);
    else if (item.property > 0)
        DrawText(x, y, shortenstring(item.name), COLOR_WHITE, 1920);
}

int amount;

int GetArrayAmount(int menu_entry){
    int i = 0;

    while (menu_objects[menu_entry].items[i].name != NULL)
        i++;
    
    return i;
}

void ReloadMenu(){
    selection = 1;
    offset = 0;
    amount = GetArrayAmount(currentmenu);

    while (menu_objects[currentmenu].items[selection + offset - 1].property <= 0 && selection + offset < amount)
        selection++;
}

u64 kDown;
u64 GetControllerInput(){
    return kDown;
}

void MakeBasicMenu(){
    int ypos = 150;
    ReloadMenu();

    while (1){
        hidScanInput();
        kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);
        ClearScreenWithElements();

        if ((kDown & KEY_LSTICK_UP || kHeld & KEY_RSTICK_UP || kDown & KEY_DUP) && selection + offset > 1){
            selection--;
            while (menu_objects[currentmenu].items[selection + offset - 1].property <= 0 && selection + offset > 1)
                selection--;
        }
            
        if ((kDown & KEY_LSTICK_DOWN || kHeld & KEY_RSTICK_DOWN || kDown & KEY_DDOWN) && selection + offset < amount){
            selection++;
            while (menu_objects[currentmenu].items[selection + offset - 1].property <= 0 && selection + offset < amount)
                selection++;
        }
            
        if (menu_objects[currentmenu].items[selection + offset - 1].property <= 0){
            if (selection + offset == amount)
                while (menu_objects[currentmenu].items[selection + offset - 1].property <= 0)
                    selection--;

            if (selection + offset <= 1)
                while (menu_objects[currentmenu].items[selection + offset - 1].property <= 0)
                    selection++;
        }

        if (selection + offset > amount)
            selection = amount - offset;

        else if (selection > MAX_ENTRIES)
            offset += selection - MAX_ENTRIES, selection = MAX_ENTRIES;

        if (selection < 1 && offset > 0)
            offset += selection - 1, selection = 1;

        else if (selection < 1 && offset <= 0)
            selection = 1;

        if (kDown & KEY_L)
            if (ChangeTopMenu(false)){
                ReloadMenu();
            }
                
        if (kDown & KEY_R)
            if (ChangeTopMenu(true)){
                ReloadMenu();
            }
                
        if (kDown & KEY_PLUS)
            break;

        if (kDown & menu_objects[currentmenu].controller_mask){
            SignalThread();
            svcSleepThread(50000);
        }
            
        ypos = 150;
        int temp = 0;
        for (int i = 0; i < ((amount > MAX_ENTRIES) ? MAX_ENTRIES + temp: amount); i++){
            DrawMenuEntry(menu_objects[currentmenu].items[i + offset], 90, ypos, (i == selection - 1));

            if (menu_objects[currentmenu].items[i + offset].property >= 0)
                ypos += 50;
            else
                temp++;
        }

        DrawTopMenu();
        DrawNotification();
        RenderFrame();
    }
}
