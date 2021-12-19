
#ifndef __ALICLOUD_ENTRY_H__
#define __ALICLOUD_ENTRY_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef struct {
    int argc;
    char **argv;
}app_main_paras_t;

int linkkit_main(int argc, char **argv);
void cmd_alicloud_linkkit(int argc, char **argv);

#endif
