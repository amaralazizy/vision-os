#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64
#define CV_PREFIX "cv-"

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
    
    // Build path to Python script
    char script_path[256];
    snprintf(script_path, sizeof(script_path), "apps/cv_%s.py", cv_cmd);
    
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
 * Execute standard command using execvp
 */
void execute_standard_command(char **args) {
    execvp(args[0], args);
    
    // If execvp returns, there was an error
    perror("execvp failed");
    exit(1);
}

/**
 * Main shell loop
 */
int main() {
    char input[MAX_INPUT];
    char *args[MAX_ARGS];
    
    printf("VisionOS Shell v1.0\n");
    printf("Type 'exit' to quit\n");
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
        
        // Parse input into arguments
        int argc = parse_input(input, args);
        
        if (argc == 0) {
            continue;
        }
        
        // Check for exit command
        if (strcmp(args[0], "exit") == 0) {
            printf("Exiting VisionOS Shell...\n");
            break;
        }
        
        // Fork process to execute command
        pid_t pid = fork();
        
        if (pid < 0) {
            // Fork failed
            perror("fork failed");
            continue;
        } else if (pid == 0) {
            // Child process
            if (is_cv_command(args[0])) {
                // Execute cv- command with Python
                execute_cv_command(args);
            } else {
                // Execute standard command
                execute_standard_command(args);
            }
        } else {
            // Parent process - wait for child to complete
            int status;
            waitpid(pid, &status, 0);
            
            if (WIFEXITED(status)) {
                int exit_status = WEXITSTATUS(status);
                if (exit_status != 0) {
                    // Command failed (non-zero exit)
                    // Error message already printed by child
                }
            }
        }
    }
    
    return 0;
}
