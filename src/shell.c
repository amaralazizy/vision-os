#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <dirent.h>
#include "visionos.h"

char *command_generator(const char *text, int state) {
    static int list_index, len;
    static CompletionPhase phase;
    char *name;
    static DIR *dir_apps = NULL;
    static DIR *dir_scripts = NULL;
    struct dirent *entry;

    char *builtins[] = {"history", "clear-history", "mem-stats", "exit", "vls", "cd", NULL};

    if (state == 0) {
        list_index = 0;
        len = strlen(text);
        phase = PHASE_BUILTINS;
        if (dir_apps) { closedir(dir_apps); dir_apps = NULL; }
        if (dir_scripts) { closedir(dir_scripts); dir_scripts = NULL; }
        
        char apps_path[1024];
        get_apps_path(apps_path, sizeof(apps_path));
        dir_apps = opendir(apps_path);
        
        char scripts_path[1024];
        snprintf(scripts_path, sizeof(scripts_path), "%s/../bash_scripts", apps_path);
        dir_scripts = opendir(scripts_path);
    }

    // Phase 0: Builtins
    if (phase == PHASE_BUILTINS) {
        while ((name = builtins[list_index])) {
            list_index++;
            if (strncmp(name, text, len) == 0) {
                return strdup(name);
            }
        }
        phase++; // Move to next phase
    }

    // Phase 1: Apps (cv-*)
    if (phase == PHASE_APPS) {
        if (dir_apps) {
            while ((entry = readdir(dir_apps))) {
                if (strncmp(entry->d_name, "cv_", 3) == 0 && strstr(entry->d_name, ".py")) {
                    char cmd[256];
                    snprintf(cmd, sizeof(cmd), "cv-%s", entry->d_name + 3);
                    char *dot = strrchr(cmd, '.');
                    if (dot) *dot = '\0';
                    
                    if (strncmp(cmd, text, len) == 0) {
                        return strdup(cmd);
                    }
                }
            }
        }
        phase++;
    }

    // Phase 2: Scripts (sh-*)
    if (phase == PHASE_SCRIPTS) {
        if (dir_scripts) {
            while ((entry = readdir(dir_scripts))) {
                if (strncmp(entry->d_name, "sh_", 3) == 0 && strstr(entry->d_name, ".sh")) {
                    char cmd[256];
                    snprintf(cmd, sizeof(cmd), "sh-%s", entry->d_name + 3);
                    char *dot = strrchr(cmd, '.');
                    if (dot) *dot = '\0';
                    
                    if (strncmp(cmd, text, len) == 0) {
                        return strdup(cmd);
                    }
                }
            }
        }
        phase++;
    }

    return NULL;
}

char **visionos_completion(const char *text, int start, int end) {
    (void)end;
    if (start == 0) {
        return rl_completion_matches(text, command_generator);
    }
    return NULL;
}

void setup_shell(void) {
    rl_attempted_completion_function = visionos_completion;
}
