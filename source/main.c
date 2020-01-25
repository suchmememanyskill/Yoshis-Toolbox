#include <stdlib.h>
#include <stdio.h>
#include <switch.h>
#include <dirent.h> 
#include <unistd.h>
#include <sys/stat.h>
#include "menu/sdl_helper.h"
#include "menu/menu.h"
#include "tools/payload.h"
#include "utils/config.h"
#include "utils/utils.h"
#include "tools/options.h"
#include "utils/config.h"
#include "tools/ams_config.h"

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
                    switch (GetCurrentMenu()){
                        case MENU_PAYLOAD:;
                            HandlePayload();
                            break;
                        case MENU_OPTIONS:
                            HandleOptions();
                            break;
                    }
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

menu temp = {"NULL", NULL, 0};

void MakeEmptyConfig(){
    FILE *fp;

    fp = fopen(INILOC, "w+");
    fprintf(fp, "[Payload]\nPath=/.\nFav=\n");

    fclose(fp);
}

int main(int argc, char *argv[]){
    Thread thread;
    Result rc;

    rc = plInitialize();
    SDLInit();

    ueventCreate(&g_Event, false);
    ueventCreate(&g_ExitEvent, false);
    rc = threadCreate(&thread, ThreadExec, NULL, NULL, 0x10000, 0x2C, -2);

    InitTopMenu();
    AddTopMenu(temp);
    AddTopMenu(temp);
    AddTopMenu(temp);

    if (access(INILOC, F_OK) == -1)
        MakeEmptyConfig();

    ReadConfig(INILOC);
    Payload_Init();
    Options_Init();
    InitAmsConfigMenu();
    
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