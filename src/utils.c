#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "visionos.h"

void get_apps_path(char *buffer, size_t size) {
    char exe_path[1024];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len != -1) {
        exe_path[len] = '\0';
        char *last_slash = strrchr(exe_path, '/');
        if (last_slash) *last_slash = '\0';
        snprintf(buffer, size, "%s/apps", exe_path);
    } else {
        strncpy(buffer, "apps", size);
    }
}

int parse_input(char *input, char **args) {
    int argc = 0;
    char *token = strtok(input, " \t\n");
    while (token != NULL && argc < MAX_ARGS - 1) {
        args[argc++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[argc] = NULL;
    return argc;
}