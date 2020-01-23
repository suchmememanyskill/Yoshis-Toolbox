#pragma once

typedef int (*ini_handler)(const char* section, const char* name, const char* value);
int ini_parse(const char* filename, ini_handler handler);