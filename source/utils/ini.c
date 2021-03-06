#include <switch.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ini.h"

char section[255], key[255], value[255];

void SplitString(char *in, char split){
    int i = 0, pos = 0, length = strlen(in);

    while (i < length && in[i] != split){
        if (in[i] == ' '){
            i++;
            continue;
        }

        key[pos] = in[i];
        pos++, i++;
    }

    key[pos] = '\0';
    pos = 0, i++;

    while (i < length){
        if (in[i] == ' ' || in[i] == '\n' || in[i] == '\r'){
            i++;
            continue;
        }
            
        value[pos] = in[i];
        i++, pos++;
    }

    value[pos] = '\0';
}

void RemoveBrackets(char *in){
    strcpy(section, in + 1);
    for (int i = 0; i < strlen(section); i++){
        if (section[i] == ']'){
            section[i] = '\0';
            break;
        }
    }
}

int ini_parse(const char* filename, ini_handler handler) {
    FILE *file;
    char temp[255], c;
    if ((file = fopen(filename, "r")) == NULL)
        return -1;

    while (1){
        if ((c = fgetc(file)) == 255)
            break;

        switch (c){
            case '\r':
            case '\n':
                break;
            case ';':
                fgets(temp, 255, file);
                break;
            case '[':
                fseek(file, -1, SEEK_CUR);
                fgets(temp, 255, file);
                RemoveBrackets(temp);
                break;
            case (char)255:
                goto out;
            default:
                fseek(file, -1, SEEK_CUR);
                fgets(temp, 255, file);
                SplitString(temp, '=');
                handler(section, key, value);
                break;
        }
    }
    out:;

    fclose(file);
    return 0;
}