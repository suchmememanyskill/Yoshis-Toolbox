#include <stdlib.h>
#include <stdio.h>
#include <switch.h>
#include <dirent.h> 
#include <sys/stat.h>
#include <string.h>
#include "payload.h"
#include "sdl_helper.h"
#include "menu.h"
#include "utils.h"


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
        while (folderitems[i].property > 0){
            if (folderitems[i].name != NULL){
                free(folderitems[i].name);
                folderitems[i].name = NULL;
            }
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

char *folder;
void SetFolder(char *in){
    if (folder != NULL)
        free(folder);
    
    folder = malloc(strlen(in) + 1);
    strcpy(folder, in);
}

void ReadFolder(){
    int amount = FolderAmount(folder, ".bin");
    CreateFolderItems(amount);

    struct dirent *de;
    DIR *dr = opendir(folder);
    int i = 0;

    while ((de = readdir(dr)) != NULL){
        if (strstr(de->d_name, ".bin") != NULL){
            folderitems[i++] = MakeMenuItem(de->d_name, 1);
        }
    }

    closedir(dr);
}

void SetMenuPayload(){
    static menu payloadmenu = {"Payload", NULL};
    payloadmenu.items = folderitems;
    EditTopMenu(payloadmenu, MENU_PAYLOAD);
}