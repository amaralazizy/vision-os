#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include "visionos.h"

/**
 * Validate that a string is not NULL and not empty
 */
int validate_string_not_empty(const char *str) {
    if (str == NULL) {
        fprintf(stderr, "Validation error: NULL string\n");
        return 0;
    }
    if (strlen(str) == 0) {
        fprintf(stderr, "Validation error: Empty string\n");
        return 0;
    }
    return 1;
}

/**
 * Validate string length is within bounds
 */
int validate_string_length(const char *str, size_t max_len) {
    if (!validate_string_not_empty(str)) return 0;
    
    size_t len = strlen(str);
    if (len > max_len) {
        fprintf(stderr, "Validation error: String length %zu exceeds maximum %zu\n", len, max_len);
        return 0;
    }
    return 1;
}

/**
 * Validate buffer pointer and size
 */
int validate_buffer(const void *buffer, size_t size) {
    if (buffer == NULL) {
        fprintf(stderr, "Validation error: NULL buffer\n");
        return 0;
    }
    if (size == 0) {
        fprintf(stderr, "Validation error: Zero buffer size\n");
        return 0;
    }
    if (size > MAX_BUFFER_SIZE) {
        fprintf(stderr, "Validation error: Buffer size %zu exceeds maximum %d\n", size, MAX_BUFFER_SIZE);
        return 0;
    }
    return 1;
}

/**
 * Validate argument array
 */
int validate_args(char **args) {
    if (args == NULL) {
        fprintf(stderr, "Validation error: NULL argument array\n");
        return 0;
    }
    if (args[0] == NULL) {
        fprintf(stderr, "Validation error: No command provided\n");
        return 0;
    }
    
    // Count and validate individual arguments
    int count = 0;
    while (args[count] != NULL) {
        if (!validate_string_length(args[count], MAX_ARG_LENGTH)) {
            fprintf(stderr, "Validation error: Argument %d is invalid\n", count);
            return 0;
        }
        count++;
        if (count >= MAX_ARGS) {
            fprintf(stderr, "Validation error: Too many arguments (max %d)\n", MAX_ARGS);
            return 0;
        }
    }
    
    return 1;
}

/**
 * Validate command name (no special characters except -, _)
 */
int validate_command_name(const char *cmd) {
    if (!validate_string_not_empty(cmd)) return 0;
    
    // Check for dangerous characters
    const char *dangerous = ";|&$()<>[]{}*?!`\\\"'";
    for (size_t i = 0; cmd[i] != '\0'; i++) {
        if (strchr(dangerous, cmd[i]) != NULL) {
            fprintf(stderr, "Validation error: Command contains dangerous character '%c'\n", cmd[i]);
            return 0;
        }
    }
    
    return validate_string_length(cmd, MAX_COMMAND_LENGTH);
}

/**
 * Validate file path (basic security checks)
 */
int validate_file_path(const char *path) {
    if (!validate_string_not_empty(path)) return 0;
    if (!validate_string_length(path, MAX_PATH_LENGTH)) return 0;
    
    // Check for null bytes (common in path traversal attacks)
    if (memchr(path, '\0', strlen(path)) != NULL) {
        fprintf(stderr, "Validation error: Path contains null bytes\n");
        return 0;
    }
    
    // Warn about suspicious patterns (but don't block entirely)
    if (strstr(path, "..") != NULL) {
        fprintf(stderr, "Warning: Path contains '..'\n");
    }
    
    return 1;
}

/**
 * Validate integer in range
 */
int validate_int_range(int value, int min, int max) {
    if (value < min || value > max) {
        fprintf(stderr, "Validation error: Value %d out of range [%d, %d]\n", value, min, max);
        return 0;
    }
    return 1;
}

/**
 * Validate and parse integer from string
 */
int validate_parse_int(const char *str, int *result) {
    if (!validate_string_not_empty(str)) return 0;
    
    char *endptr;
    errno = 0;
    long val = strtol(str, &endptr, 10);
    
    // Check for conversion errors
    if (errno == ERANGE || val > INT_MAX || val < INT_MIN) {
        fprintf(stderr, "Validation error: Integer overflow in '%s'\n", str);
        return 0;
    }
    
    if (endptr == str) {
        fprintf(stderr, "Validation error: No digits found in '%s'\n", str);
        return 0;
    }
    
    if (*endptr != '\0') {
        fprintf(stderr, "Validation error: Invalid characters after number in '%s'\n", str);
        return 0;
    }
    
    *result = (int)val;
    return 1;
}

/**
 * Validate pipe count
 */
int validate_pipe_count(int count) {
    return validate_int_range(count, 0, MAX_PIPES);
}

/**
 * Validate file descriptor
 */
int validate_fd(int fd) {
    if (fd < 0) {
        fprintf(stderr, "Validation error: Invalid file descriptor %d\n", fd);
        return 0;
    }
    return 1;
}

/**
 * Validate process ID
 */
int validate_pid(pid_t pid) {
    if (pid < 0) {
        fprintf(stderr, "Validation error: Invalid process ID %d\n", pid);
        return 0;
    }
    return 1;
}

/**
 * Sanitize input string (remove control characters except newline/tab)
 */
void sanitize_input(char *str) {
    if (str == NULL) return;
    
    for (size_t i = 0; str[i] != '\0'; i++) {
        // Remove non-printable characters except tab and newline
        if (iscntrl((unsigned char)str[i]) && str[i] != '\t' && str[i] != '\n') {
            str[i] = ' ';
        }
    }
}

/**
 * Validate input command string
 */
int validate_input_command(const char *input) {
    if (!validate_string_not_empty(input)) return 0;
    if (!validate_string_length(input, SHELL_MAX_INPUT)) return 0;
    
    // Check for suspicious patterns
    size_t consecutive_special = 0;
    for (size_t i = 0; input[i] != '\0'; i++) {
        if (!isprint((unsigned char)input[i]) && input[i] != '\t' && input[i] != '\n') {
            fprintf(stderr, "Validation error: Input contains non-printable characters\n");
            return 0;
        }
        
        // Count consecutive special characters
        if (strchr("|&;<>", input[i]) != NULL) {
            consecutive_special++;
            if (consecutive_special > 3) {
                fprintf(stderr, "Validation error: Too many consecutive special characters\n");
                return 0;
            }
        } else {
            consecutive_special = 0;
        }
    }
    
    return 1;
}

/**
 * Validate memory allocation result
 */
void* validate_malloc(size_t size, const char *context) {
    if (size == 0) {
        fprintf(stderr, "Validation error: Attempted to allocate 0 bytes in %s\n", 
                context ? context : "unknown");
        return NULL;
    }
    
    if (size > MAX_ALLOCATION_SIZE) {
        fprintf(stderr, "Validation error: Allocation size %zu exceeds maximum %d in %s\n", 
                size, MAX_ALLOCATION_SIZE, context ? context : "unknown");
        return NULL;
    }
    
    void *ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "Validation error: malloc failed for %zu bytes in %s\n", 
                size, context ? context : "unknown");
    }
    
    return ptr;
}

/**
 * Validate array index
 */
int validate_array_index(int index, int array_size) {
    if (index < 0) {
        fprintf(stderr, "Validation error: Negative array index %d\n", index);
        return 0;
    }
    if (index >= array_size) {
        fprintf(stderr, "Validation error: Array index %d out of bounds (size %d)\n", 
                index, array_size);
        return 0;
    }
    return 1;
}

/**
 * Validate redirection type
 */
int validate_redirect_type(RedirectType type) {
    if (type < REDIRECT_NONE || type > REDIRECT_INPUT) {
        fprintf(stderr, "Validation error: Invalid redirect type %d\n", type);
        return 0;
    }
    return 1;
}

/**
 * Validate environment variable name
 */
int validate_env_var_name(const char *name) {
    if (!validate_string_not_empty(name)) return 0;
    
    // First character must be letter or underscore
    if (!isalpha((unsigned char)name[0]) && name[0] != '_') {
        fprintf(stderr, "Validation error: Environment variable must start with letter or underscore\n");
        return 0;
    }
    
    // Remaining characters must be alphanumeric or underscore
    for (size_t i = 1; name[i] != '\0'; i++) {
        if (!isalnum((unsigned char)name[i]) && name[i] != '_') {
            fprintf(stderr, "Validation error: Invalid character in environment variable name\n");
            return 0;
        }
    }
    
    return 1;
}
