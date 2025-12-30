#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "visionos.h"

int main() {
    char input[MAX_INPUT];
    printf("VisionOS Shell Initiated (with Memory Management).\n");
    printf("Built-in commands: history, clear-history, mem-stats, exit\n");
    printf("====================================\n\n");

    while (1) {
        printf("visionos> ");
        fflush(stdout);

        if (fgets(input, MAX_INPUT, stdin) == NULL) break;
        input[strcspn(input, "\n")] = 0;
        if (strlen(input) == 0) continue;
        
        // Check for exit first
        if (strcmp(input, "exit") == 0) {
            printf("Cleaning up and exiting...\n");
            clear_history();
            break;
        }
        
        // Add command to history (demonstrates dynamic memory allocation)
        add_to_history(input);
        
        // Handle built-in commands for memory management demonstration
        if (strcmp(input, "history") == 0) {
            show_history();
            continue;
        }
        
        if (strcmp(input, "clear-history") == 0) {
            clear_history();
            printf("History cleared.\n");
            continue;
        }
        
        if (strcmp(input, "mem-stats") == 0) {
            print_memory_stats();
            continue;
        }

        char *commands[MAX_ARGS];
        int num_cmds = 0;
        char *cmd_ptr = input;
        char *temp_cmd;

        while ((temp_cmd = strsep(&cmd_ptr, "|")) != NULL && num_cmds < MAX_ARGS) {
            if (*temp_cmd != '\0') commands[num_cmds++] = temp_cmd;
        }

        int pipefd[2];
        int prev_pipe_read = -1;

        for (int i = 0; i < num_cmds; i++) {
            char *args[MAX_ARGS];
            parse_input(commands[i], args);
            if (args[0] == NULL) continue;

            // Handle Built-ins (like cd)
            if (strcmp(args[0], "cd") == 0 && num_cmds == 1) {
                char *path = args[1] ? args[1] : getenv("HOME");
                if (chdir(path) != 0) perror("cd failed");
                continue;
            }

            if (i < num_cmds - 1) pipe(pipefd);

            pid_t pid = fork();
            if (pid == 0) {
                if (prev_pipe_read != -1) {
                    dup2(prev_pipe_read, STDIN_FILENO);
                    close(prev_pipe_read);
                }
                if (i < num_cmds - 1) {
                    dup2(pipefd[1], STDOUT_FILENO);
                    close(pipefd[1]);
                    close(pipefd[0]);
                }
                execute_command(args);
            } else {
                if (prev_pipe_read != -1) close(prev_pipe_read);
                if (i < num_cmds - 1) {
                    prev_pipe_read = pipefd[0];
                    close(pipefd[1]);
                }
            }
        }
        for (int i = 0; i < num_cmds; i++) wait(NULL);
    }
    return 0;
}