#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include "visionos.h"

int handle_builtin(char **args) {
    if (args[0] == NULL) return 0;

    if (strcmp(args[0], "exit") == 0) {
        printf("Cleaning up and exiting...\n");
        clear_history();
        exit(0);
    }
    
    if (strcmp(args[0], "history") == 0) {
        show_history();
        return 1;
    }
    
    if (strcmp(args[0], "clear-history") == 0) {
        clear_history();
        rl_clear_history();
        printf("History cleared.\n");
        return 1;
    }
    
    if (strcmp(args[0], "mem-stats") == 0) {
        print_memory_stats();
        return 1;
    }

    if (strcmp(args[0], "cd") == 0) {
        char *path = args[1] ? args[1] : getenv("HOME");
        if (chdir(path) != 0) perror("cd failed");
        return 1;
    }

    return 0;
}
