#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64
#define CV_PREFIX "cv-"

/**
 * Get the absolute path to the apps directory
 */
void get_apps_path(char *buffer, size_t size) {
    char exe_path[1024];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len != -1) {
        exe_path[len] = '\0';
        // Remove executable name to get directory
        char *last_slash = strrchr(exe_path, '/');
        if (last_slash) {
            *last_slash = '\0';
        }
        snprintf(buffer, size, "%s/apps", exe_path);
    } else {
        // Fallback to relative path if readlink fails
        strncpy(buffer, "apps", size);
    }
}

/**
 * Parse input string into arguments array
 * Returns the number of arguments parsed
 */
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

/**
 * Check if command starts with cv- prefix
 */
int is_cv_command(const char *cmd) {
    return strncmp(cmd, CV_PREFIX, strlen(CV_PREFIX)) == 0;
}

/**
 * Execute cv- command using Python script
 */
void execute_cv_command(char **args) {
    // Extract command name (e.g., "cv-show" -> "show")
    const char *cv_cmd = args[0] + strlen(CV_PREFIX);
    
    // Get apps directory path
    char apps_path[1024];
    get_apps_path(apps_path, sizeof(apps_path));

    // Build path to Python script
    char script_path[2048];
    snprintf(script_path, sizeof(script_path), "%s/cv_%s.py", apps_path, cv_cmd);
    
    // Prepare arguments for Python script
    // Format: python3 apps/cv_show.py [args...]
    char *py_args[MAX_ARGS + 2];
    py_args[0] = "python3";
    py_args[1] = script_path;
    
    // Copy remaining arguments
    int i = 1;
    while (args[i] != NULL && i < MAX_ARGS - 1) {
        py_args[i + 1] = args[i];
        i++;
    }
    py_args[i + 1] = NULL;
    
    // Execute Python script
    execvp("python3", py_args);
    
    // If execvp returns, there was an error
    perror("execvp failed");
    exit(1);
}

/**
 * Check if command is vls
 */
int is_vls_command(const char *cmd) {
    return strcmp(cmd, "vls") == 0;
}

/**
 * Execute vls command using Python script
 */
void execute_vls_command(char **args) {
    // Get apps directory path
    char apps_path[1024];
    get_apps_path(apps_path, sizeof(apps_path));

    // Build path to Python script
    char script_path[2048];
    snprintf(script_path, sizeof(script_path), "%s/vls.py", apps_path);

    // Prepare arguments for Python script
    // Format: python3 apps/vls.py [args...]
    char *py_args[MAX_ARGS + 2];
    py_args[0] = "python3";
    py_args[1] = script_path;
    
    // Copy remaining arguments (skipping "vls")
    int i = 1;
    int py_idx = 2;
    while (args[i] != NULL && i < MAX_ARGS - 1) {
        py_args[py_idx++] = args[i];
        i++;
    }
    py_args[py_idx] = NULL;
    
    // Execute Python script
    execvp("python3", py_args);
    
    // If execvp returns, there was an error
    perror("execvp failed");
    exit(1);
}

/**
 * Execute standard command using execvp
 */
void execute_standard_command(char **args) {
    execvp(args[0], args);
    
    // If execvp returns, there was an error
    perror("execvp failed");
    exit(1);
}

/**
 * Helper to route command execution
 */
void execute_command(char **args) {
    if (args[0] == NULL) exit(0);
    
    if (is_cv_command(args[0])) {
        execute_cv_command(args);
    } else if (is_vls_command(args[0])) {
        execute_vls_command(args);
    } else {
        execute_standard_command(args);
    }
}

/**
 * Main shell loop
 */
int main() {
    char input[MAX_INPUT];
    
    printf("VisionOS Shell Initiated.\n");
    printf("Type 'exit' to quit.\n");
    printf("====================================\n\n");
    
    // Infinite loop for CLI prompt
    while (1) {
        printf("visionos> ");
        fflush(stdout);
        
        // Read input
        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            printf("\n");
            break;
        }
        
        // Remove trailing newline
        input[strcspn(input, "\n")] = 0;
        
        // Skip empty input
        if (strlen(input) == 0) {
            continue;
        }
        
        // Check for exit command before parsing pipeline
        if (strcmp(input, "exit") == 0) {
            printf("Exiting VisionOS Shell...\n");
            break;
        }

        // Split by pipe '|'
        char *commands[MAX_ARGS];
        int num_cmds = 0;
        char *cmd_ptr = input;
        char *temp_cmd;
        
        // Use strsep to split by pipe
        while ((temp_cmd = strsep(&cmd_ptr, "|")) != NULL && num_cmds < MAX_ARGS) {
            if (*temp_cmd != '\0') {
                commands[num_cmds++] = temp_cmd;
            }
        }

        int pipefd[2];
        int prev_pipe_read = -1;

        for (int i = 0; i < num_cmds; i++) {
            // Parse args for this command segment
            char *args[MAX_ARGS];
            parse_input(commands[i], args);
            
            if (args[0] == NULL) continue;

            // Handle cd command (only if it's the only command in the pipeline)
            if (strcmp(args[0], "cd") == 0 && num_cmds == 1) {
                if (args[1] == NULL) {
                    // cd with no args -> go to HOME
                    char *home = getenv("HOME");
                    if (home) {
                        if (chdir(home) != 0) {
                            perror("cd failed");
                        }
                    } else {
                        fprintf(stderr, "cd: HOME not set\n");
                    }
                } else {
                    if (chdir(args[1]) != 0) {
                        perror("cd failed");
                    }
                }
                continue;
            }

            // Create pipe if not the last command
            if (i < num_cmds - 1) {
                if (pipe(pipefd) < 0) {
                    perror("pipe failed");
                    break;
                }
            }

            pid_t pid = fork();
            
            if (pid < 0) {
                perror("fork failed");
                break;
            } else if (pid == 0) {
                // Child process
                
                // Redirect stdin from previous pipe
                if (prev_pipe_read != -1) {
                    if (dup2(prev_pipe_read, STDIN_FILENO) < 0) {
                        perror("dup2 stdin failed");
                        exit(1);
                    }
                    close(prev_pipe_read);
                }
                
                // Redirect stdout to current pipe
                if (i < num_cmds - 1) {
                    if (dup2(pipefd[1], STDOUT_FILENO) < 0) {
                        perror("dup2 stdout failed");
                        exit(1);
                    }
                    close(pipefd[1]);
                    close(pipefd[0]); // Close read end in child
                }
                
                execute_command(args);
            } else {
                // Parent process
                
                // Close previous read end
                if (prev_pipe_read != -1) {
                    close(prev_pipe_read);
                }
                
                // Set up next read end
                if (i < num_cmds - 1) {
                    prev_pipe_read = pipefd[0];
                    close(pipefd[1]); // Close write end in parent
                }
            }
        }
        
        // Wait for all children to finish
        for (int i = 0; i < num_cmds; i++) {
            wait(NULL);
        }
    }
    
    return 0;
}
