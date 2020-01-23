#include <stdlib.h>
#include <stdio.h>
#include <switch.h>
#include <dirent.h> 
#include <sys/stat.h>
#include <string.h>
#include "payload.h"
#include "options.h"
#include "../menu/sdl_helper.h"
#include "../menu/menu.h"
#include "../utils/utils.h"
#include "../utils/config.h"

menu_item options[10];

void SetMenuOptions(){
    static menu optionsmenu = {"Options", NULL};
    optionsmenu.items = options;
    EditTopMenu(optionsmenu, MENU_OPTIONS);
}

void BuildOptionsMenu(){
    int i = 0;

    options[i++] = MakeMenuItem("-- Payload Location --", 0);
    options[i++] = MakeMenuItem("/payloads/", 1);

    if (checkfolder("/bootloader/payloads/."))
        options[i++] = MakeMenuItem("/bootloader/payloads/", 2);

    if (checkfolder("/argon/payloads/."))
        options[i++] = MakeMenuItem("/argon/payloads/", 3);

    if (checkfolder("/."))
        options[i++] = MakeMenuItem("/", 4);

    options[i++] = MakeMenuItem("-- Extras --", 0);
    options[i++] = MakeMenuItem("Reboot to RCM", 5);
    options[i++] = MakeMenuItem("Reboot to atmosphere/reboot_payload.bin", 6);
    options[i++] = (menu_item) {NULL, -1};
}

void Options_Init(){
    BuildOptionsMenu();
    SetMenuOptions();
}

void HandleOptions(){
    menu_item item = GetCurrentElement();

    switch (item.property){
        case 1:;
        case 2:;
        case 3:;
        case 4:;
            char *temp = makestring(addstrings(item.name, "."));
            SetPayloadFolder(temp);
            ReadFolder();
            WriteConfig();
            free(temp);
            MakeNotification(addstrings(item.name, " has been set as payload folder"), 200, COLOR_WHITE);
            break;
    }
}