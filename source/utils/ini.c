#include <switch.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ini.h"

char key[255], value[255];

void SplitString(char *in, char split){
    int i = 0, pos = 0, length = strlen(in);

    while (i < length && in[i] != split){
        key[i] = in[i];
        i++;
    }
    key[i] = '\0';
    i++;

    while (i < length){
        value[pos] = in[i];
        i++, pos++;
    }

    value[pos] = '\0';
}

int ini_parse(const char* filename, ini_handler handler) {
    FILE *file;
    char section[255], temp[255], c;
    file = fopen(filename, "r");

    while (1){
        if ((c = fgetc(file)) == 255)
            break;

        printf("c=%c(%d)\n", c, (int)c);

        switch (c){
            case '\n':
                break;
            case ';':
                fscanf(file, "%s\n", key);
                break;
            case '[':
                fseek(file, -1, SEEK_CUR);
                fscanf(file, "[%s]\n", section);
                section[strlen(section) - 1] = '\0';
                break;
            case (char)255:
                goto out;
            default:
                fseek(file, -1, SEEK_CUR);
                fscanf(file, "%s\n", temp);
                SplitString(temp, '=');
                handler(section, key, value);
                break;
        }
    }
    out:;

    fclose(file);
    return 0;
}