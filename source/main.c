#include <stdlib.h>
#include <stdio.h>
#include <switch.h>
#include <dirent.h> 
#include <sys/stat.h>
#include "sdl_helper.h"
#include "menu.h"
#include "payload.h"

static UEvent g_Event;
static UEvent g_ExitEvent;
static bool isThreadBusy = false;

void ThreadExec(void *arg){
    Result rc;
    int idx;
    bool isExit = false;

    while (!isExit){
        isThreadBusy = false;
        rc = waitMulti(&idx, -1, waiterForUEvent(&g_Event), waiterForUEvent(&g_ExitEvent));
        isThreadBusy = true;

        if (R_SUCCEEDED(rc)){
            switch (idx){
                case 0:;
                    menu_item item = GetCurrentElement();

                    MakeNotification(item.name, 200, COLOR_WHITE);

                    ueventClear(&g_Event);
                    break;
                case 1:
                    isExit = true;
                    break;
            }
        }
    }
}

int SignalThread(){
    if (isThreadBusy)
        return -1;
    else {
        ueventSignal(&g_Event);
        return 0;
    }
}

menu_item menu1items[5] = {
    {"This", 1},
    {"Is", 1},
    {"A", 1},
    {"Test", 1},
    {"no", -1}
};

menu_item menu2items[7] = {
    {"This", 1},
    {"Is", 0},
    {"A", 1},
    {"Test", 1},
    {"Yeet", 1},
    {"Yote", 1},
    {"no", -1}
};


menu menu1 = {"Menu1", menu1items};
menu menu2 = {"Menu2", menu2items};
menu menu3 = {"Payload", NULL};

int main(int argc, char *argv[]){
    Thread thread;
    Result rc;

    rc = plInitialize();
    SDLInit();

    ueventCreate(&g_Event, false);
    ueventCreate(&g_ExitEvent, false);
    rc = threadCreate(&thread, ThreadExec, NULL, NULL, 0x10000, 0x2C, -2);

    InitTopMenu();
    AddTopMenu(menu1);
    AddTopMenu(menu2);
    AddTopMenu(menu3);

    SetFolder("/payloads/.");
    ReadFolder();
    SetMenuPayload();
    
    if (R_SUCCEEDED(rc)){
        rc = threadStart(&thread);

        if (R_SUCCEEDED(rc)){
            MakeBasicMenu();
            ueventSignal(&g_ExitEvent);
            threadWaitForExit(&thread);
            threadClose(&thread);
        }
    }


    SDLDeInit();
    plExit();
    return 0;
}