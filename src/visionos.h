#ifndef VISIONOS_H
#define VISIONOS_H

#define MAX_INPUT 1024
#define MAX_ARGS 64
#define CV_PREFIX "cv-"
#define SH_PREFIX "sh-"

// Utils
void get_apps_path(char *buffer, size_t size);
int parse_input(char *input, char **args);

// Executor
void execute_command(char **args);

#endif