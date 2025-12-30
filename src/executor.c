#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "visionos.h"

static int is_cv_command(const char *cmd) { 
    return strncmp(cmd, CV_PREFIX, strlen(CV_PREFIX)) == 0; 
}
static int is_sh_command(const char *cmd) { 
    return strncmp(cmd, SH_PREFIX, strlen(SH_PREFIX)) == 0; 
}
static int is_vls_command(const char *cmd) { 
    return strcmp(cmd, "vls") == 0; 
}

void execute_command(char **args) {
    if (args[0] == NULL) exit(0);

    char apps_path[1024];
    get_apps_path(apps_path, sizeof(apps_path));
    char script_path[2048];

    if (is_cv_command(args[0])) {
        const char *cv_cmd = args[0] + strlen(CV_PREFIX);
        snprintf(script_path, sizeof(script_path), "%s/cv_%s.py", apps_path, cv_cmd);
        
        char *py_args[MAX_ARGS + 2];
        py_args[0] = "python3";
        py_args[1] = script_path;
        int i = 1;
        while (args[i] != NULL) { py_args[i + 1] = args[i]; i++; }
        py_args[i + 1] = NULL;
        execvp("python3", py_args);

    } else if (is_vls_command(args[0])) {
        snprintf(script_path, sizeof(script_path), "%s/vls.py", apps_path);
        char *py_args[MAX_ARGS + 2];
        py_args[0] = "python3";
        py_args[1] = script_path;
        int i = 1;
        while (args[i] != NULL) { py_args[i + 1] = args[i]; i++; }
        py_args[i + 1] = NULL;
        execvp("python3", py_args);

    } else if (is_sh_command(args[0])) {
        const char *sh_cmd = args[0] + strlen(SH_PREFIX);
        // Using apps_path logic for consistency
        snprintf(script_path, sizeof(script_path), "%s/../bash_scripts/sh_%s.sh", apps_path, sh_cmd);
        char *sh_args[MAX_ARGS + 2];
        sh_args[0] = "bash";
        sh_args[1] = script_path;
        int i = 1;
        while (args[i] != NULL) { sh_args[i + 1] = args[i]; i++; }
        sh_args[i + 1] = NULL;
        execvp("bash", sh_args);

    } else {
        execvp(args[0], args);
    }

    perror("Execution failed");
    exit(1);
}