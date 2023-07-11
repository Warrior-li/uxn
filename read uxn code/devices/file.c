//
// Created by Warrior on 08/07/2023.
//
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef _WIN32
#include <libiberty/libiberty.h>
#define realpath(s, dummy) lrealpath(s)
#define DIR_SEP_CHAR '\\'
#define DIR_SEP_STR "\\"
#define pathcmp(path1, path2, length) strncasecmp(path1, path2, length) /* strncasecmp provided by libiberty */
#define notdriveroot(file_name) (file_name[0] != DIR_SEP_CHAR && ((strlen(file_name) > 2 && file_name[1] != ':') || strlen(file_name) <= 2))
#else
#define DIR_SEP_CHAR '/'
#define DIR_SEP_STR "/"
#define pathcmp(path1, path2, length) strncmp(path1, path2, length)
#define notdriveroot(file_name) (file_name[0] != DIR_SEP_CHAR)
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#include "../uxn.h"
#include "file.h"

typedef struct {
    FILE *f;
    DIR *dir;
    char current_filename[4096];
    struct dirent *de;
    enum { IDLE,
        FILE_READ,
        FILE_WRITE,
        DIR_READ } state;
    int outside_sandbox;
} UxnFile;