#include <stdlib.h>
#include <stdio.h>
#include <switch.h>
#include <dirent.h> 
#include <sys/stat.h>
#include <string.h>
#include "payload.h"
#include "../menu/sdl_helper.h"
#include "../menu/menu.h"
#include "../utils/utils.h"
#include "../utils/config.h"
#include <unistd.h>


// ATMOSPHERE CODE -----------------------------------------------------------------

#define IRAM_PAYLOAD_MAX_SIZE 0x2F000
#define IRAM_PAYLOAD_BASE 0x40010000

static alignas(0x1000) u8 g_reboot_payload[IRAM_PAYLOAD_MAX_SIZE];
static alignas(0x1000) u8 g_ff_page[0x1000];
static alignas(0x1000) u8 g_work_page[0x1000];

void do_iram_dram_copy(void *buf, uintptr_t iram_addr, size_t size, int option) {
    memcpy(g_work_page, buf, size);
    
    SecmonArgs args = {0};
    args.X[0] = 0xF0000201;             /* smcAmsIramCopy */
    args.X[1] = (uintptr_t)g_work_page;  /* DRAM Address */
    args.X[2] = iram_addr;              /* IRAM Address */
    args.X[3] = size;                   /* Copy size */
    args.X[4] = option;                 /* 0 = Read, 1 = Write */
    svcCallSecureMonitor(&args);
    
    memcpy(buf, g_work_page, size);
}

void copy_to_iram(uintptr_t iram_addr, void *buf, size_t size) {
    do_iram_dram_copy(buf, iram_addr, size, 1);
}

void copy_from_iram(void *buf, uintptr_t iram_addr, size_t size) {
    do_iram_dram_copy(buf, iram_addr, size, 0);
}

static void clear_iram(void) {
    memset(g_ff_page, 0xFF, sizeof(g_ff_page));
    for (size_t i = 0; i < IRAM_PAYLOAD_MAX_SIZE; i += sizeof(g_ff_page)) {
        copy_to_iram(IRAM_PAYLOAD_BASE + i, g_ff_page, sizeof(g_ff_page));
    }
}

static void reboot_to_payload(void) {
    clear_iram();
    
    for (size_t i = 0; i < IRAM_PAYLOAD_MAX_SIZE; i += 0x1000) {
        copy_to_iram(IRAM_PAYLOAD_BASE + i, &g_reboot_payload[i], 0x1000);
    }
    
    splSetConfig((SplConfigItem)65001, 2);
}

// END ATMOSPHERE CODE -----------------------------------------------------------------

int reboot(const char *payloc){
    Result rc = splInitialize();

    if (R_FAILED(rc)) return 1;

    FILE *payload = fopen(payloc, "rb");
    
    if (payload == NULL){
        //MakeNotification("Payload location invalid!", 200, COLOR_RED, COLOR_BLACK);
        return 2;
    }
    else {
        fread(g_reboot_payload, 1, sizeof(g_reboot_payload), payload);
        fclose(payload);
        reboot_to_payload();
    }

    splExit();
    return 0;
}

int reboottoRCM(){
    Result rc = splInitialize();

    if (R_FAILED(rc)) 
        return 1;

    rc = splSetConfig((SplConfigItem) 65001, 1);

    if (R_FAILED(rc)) 
        return 2;
    
    return 3;
}

menu_item *folderitems = NULL;

void ClearFolderItems(){
    int i = 0;

    if (folderitems != NULL){
        while (folderitems[i].name != NULL){
            free(folderitems[i].name);
            folderitems[i].name = NULL;
            i++;
        }
        free(folderitems);
        folderitems = NULL;
    }
}

void CreateFolderItems(int amount){
    ClearFolderItems();
    folderitems = calloc(amount + 1, sizeof(menu_item));
    folderitems[amount] = (menu_item) {NULL, -1};   
}

payload_config payloadconfig;

void SetPayloadConfig(){
    payloadconfig = get_payload_config();
}

void Payload_Init(){
    ReadPayloadConfig(INILOC);
    payloadconfig = get_payload_config();
    ReadFolder();
}

bool isValidFolder;
void ReadFolder(){
    SetTempMenuPayload();

    int amount = FolderAmount(payloadconfig.path, ".bin");
    CreateFolderItems(amount + 3);

    struct dirent *de;
    DIR *dr = opendir(payloadconfig.path);
    int i = 3;

    folderitems[0] = MakeMenuItem("-- Options --", (strlen(payloadconfig.fav) > 0) ? 0 : -1);
    folderitems[1] = MakeMenuItem("Launch favorite payload", (strlen(payloadconfig.fav) > 0) ? 1 : -1);
    folderitems[2] = MakeMenuItem("-- Payloads --", 0);

    while ((de = readdir(dr)) != NULL){
        if (strstr(de->d_name, ".bin") != NULL){
            folderitems[i++] = MakeMenuItem(de->d_name, 2);
        }
    }

    if (i == 3){
        folderitems[i] = MakeMenuItem("No payloads found!", 3);
        isValidFolder = false;
    }
    else
        isValidFolder = true;

    closedir(dr);
    SetMenuPayload();
}

void SetMenuPayload(){
    EditTopMenu((menu) {"Payload", folderitems, KEY_A}, MENU_PAYLOAD);
    ReloadMenu();
}

menu_item tempitems[2] = {
    {"Loading...", 1},
    {NULL, -1}
};

void SetTempMenuPayload(){
    EditTopMenu((menu) {"Payload", tempitems, 0}, MENU_PAYLOAD);
    ReloadMenu();
}

bool inChoiceMenu = false;

menu_item ChoiceMenuItems[7] = {
    {"Would you like to reboot to:", 0},
    {"PAYLOAD", 0},
    {"Yes", 1},
    {"No", 2},
    {"Set as favorite payload", 3},
    {"Set as atmosphere reboot payload", 4},
    {NULL, -1}
};

void SetChoiceMenuPayload(){
    EditTopMenu((menu) {"Payload", ChoiceMenuItems, KEY_A | KEY_B}, MENU_PAYLOAD);
    ReloadMenu();
}

void HandlePayload(){
    menu_item item;
    char *temp;
    static char *payloadname = NULL;

    u64 getInput = GetControllerInput();

    if (getInput & KEY_B)
        item = (menu_item) {"back", 2};
    else
        item = GetCurrentElement();

    if (inChoiceMenu){
        switch (item.property){
            case 1:;
                temp = makestring(payloadconfig.path);
                temp[strlen(temp) - 1] = '\0';
                reboot(addstrings(temp, payloadname));
                MakeNotification("Payload location invalid!", 200, COLOR_RED);
                free(temp);
                break;
            case 2:;
                inChoiceMenu = false;
                ReadFolder();
                break;
            case 3:;
                temp = makestring(payloadconfig.path);
                temp[strlen(temp) - 1] = '\0';
                SetPayloadFav(addstrings(temp, payloadname));
                WriteConfig();
                MakeNotification("Set payload as favorite!", 200, COLOR_WHITE);
                free(temp);
                break;
            case 4:;
                temp = makestring(payloadconfig.path);
                temp[strlen(temp) - 1] = '\0';
                if (copy(addstrings(temp, payloadname), "/atmosphere/reboot_payload.bin"))
                    MakeNotification("Copy failed!", 200, COLOR_RED);
                else
                    MakeNotification("Copy successful!", 200, COLOR_WHITE);
                break;
        }
    }
    else switch (item.property){
        case 1:;
            reboot(payloadconfig.fav);
            SetPayloadFav("");
            WriteConfig();
            ReadFolder();
            MakeNotification("Payload location invalid!", 200, COLOR_RED);
            break;
        case 2:;
            if (payloadname != NULL)
                free(payloadname);
            ChoiceMenuItems[1] = MakeMenuItem(item.name, 0);
            payloadname = makestring(item.name);
            inChoiceMenu = true;
            SetChoiceMenuPayload();
            break;
            

    }
}