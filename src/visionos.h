#ifndef VISIONOS_H
#define VISIONOS_H

#define SHELL_MAX_INPUT 1024
#define MAX_ARGS 64
#define MAX_HISTORY 100
#define CV_PREFIX "cv-"
#define SH_PREFIX "sh-"

// Enums
typedef enum {
    PHASE_BUILTINS = 0,
    PHASE_APPS,
    PHASE_SCRIPTS
} CompletionPhase;

typedef enum {
    REDIRECT_NONE = 0,
    REDIRECT_OVERWRITE, // >
    REDIRECT_APPEND,    // >>
    REDIRECT_INPUT      // <
} RedirectType;

// Utils
void get_apps_path(char *buffer, size_t size);
int parse_input(char *input, char **args);

// Executor
void execute_command(char **args);

// Shell
void setup_shell(void);
char **visionos_completion(const char *text, int start, int end);

// Builtins
int handle_builtin(char **args);

// Memory Management
void add_to_history(const char *command);
void remove_oldest_history(void);
void show_history(void);
void clear_history(void);
int get_history_count(void);
char* safe_strdup(const char *str);
char* build_path(const char *dir, const char *file);
char** allocate_args(int count);
void free_args(char **args);
void print_memory_stats(void);

#endif