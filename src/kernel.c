#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "visionos.h"

int main() {
    char *input;
    setup_signals();
    printf("VisionOS Shell Initiated (with Memory Management).\n");
    printf("Built-in commands: history, clear-history, mem-stats, exit\n");
    printf("====================================\n\n");


    setup_shell();
    
    // Allow our signals to propagate even when readline is active
    rl_catch_signals = 0;

    while (1) {
        alarm(TIMEOUT_SECONDS);
        input = readline("visionos> ");
        alarm(0);
        
        if (!input) break; // EOF
        
        if (strlen(input) > 0) {
            add_history(input);
            add_to_history(input); // Keep our custom memory management in sync
        } else {
            free(input);
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

            // Handle Built-ins
            if (num_cmds == 1 && handle_builtin(args)) {
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
                exit(0);
            } else {
                set_foreground_pid(pid);
                if (prev_pipe_read != -1) close(prev_pipe_read);
                if (i < num_cmds - 1) {
                    prev_pipe_read = pipefd[0];
                    close(pipefd[1]);
                }
            }
        }
        for (int i = 0; i < num_cmds; i++) wait(NULL);
        
        free(input);
    }
    return 0;
}