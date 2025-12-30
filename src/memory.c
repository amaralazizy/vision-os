#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "visionos.h"

// Command history structure for memory management demonstration
typedef struct HistoryNode {
    char *command;
    struct HistoryNode *next;
    struct HistoryNode *prev;
} HistoryNode;

static HistoryNode *history_head = NULL;
static HistoryNode *history_tail = NULL;
static int history_count = 0;

/**
 * Allocate memory for a new command in history
 * Demonstrates: malloc, strdup, linked list management
 */
void add_to_history(const char *command) {
    // Skip empty commands
    if (!command || strlen(command) == 0) return;
    
    // Allocate memory for new node
    HistoryNode *new_node = (HistoryNode*)malloc(sizeof(HistoryNode));
    if (!new_node) {
        fprintf(stderr, "Memory allocation failed for history node\n");
        return;
    }
    
    // Allocate and copy command string
    new_node->command = strdup(command);
    if (!new_node->command) {
        fprintf(stderr, "Memory allocation failed for command string\n");
        free(new_node);
        return;
    }
    
    new_node->next = NULL;
    new_node->prev = history_tail;
    
    // Add to linked list
    if (history_tail) {
        history_tail->next = new_node;
    } else {
        history_head = new_node;
    }
    history_tail = new_node;
    history_count++;
    
    // Limit history size to prevent unbounded memory growth
    if (history_count > MAX_HISTORY) {
        remove_oldest_history();
    }
}

/**
 * Remove oldest command from history
 * Demonstrates: proper memory deallocation, preventing memory leaks
 */
void remove_oldest_history(void) {
    if (!history_head) return;
    
    HistoryNode *old_head = history_head;
    history_head = history_head->next;
    
    if (history_head) {
        history_head->prev = NULL;
    } else {
        history_tail = NULL;
    }
    
    // Free allocated memory
    free(old_head->command);
    free(old_head);
    history_count--;
}

/**
 * Display command history
 */
void show_history(void) {
    HistoryNode *current = history_head;
    int index = 1;
    
    printf("\nCommand History:\n");
    printf("================\n");
    while (current) {
        printf("%4d  %s\n", index++, current->command);
        current = current->next;
    }
    printf("\n");
}

/**
 * Clear all history and free memory
 * Demonstrates: proper cleanup of all allocated resources
 */
void clear_history(void) {
    HistoryNode *current = history_head;
    HistoryNode *next;
    
    while (current) {
        next = current->next;
        free(current->command);
        free(current);
        current = next;
    }
    
    history_head = NULL;
    history_tail = NULL;
    history_count = 0;
}

/**
 * Get history count
 */
int get_history_count(void) {
    return history_count;
}

/**
 * Safe string duplication with error handling
 * Demonstrates: wrapper functions for safer memory operations
 */
char* safe_strdup(const char *str) {
    if (!str) return NULL;
    
    char *copy = strdup(str);
    if (!copy) {
        fprintf(stderr, "Memory allocation failed in safe_strdup\n");
        return NULL;
    }
    return copy;
}

/**
 * Allocate and build a dynamic path string
 * Demonstrates: dynamic string building with proper size calculation
 */
char* build_path(const char *dir, const char *file) {
    if (!dir || !file) return NULL;
    
    // Calculate required size: dir + "/" + file + null terminator
    size_t size = strlen(dir) + 1 + strlen(file) + 1;
    
    char *path = (char*)malloc(size);
    if (!path) {
        fprintf(stderr, "Memory allocation failed in build_path\n");
        return NULL;
    }
    
    snprintf(path, size, "%s/%s", dir, file);
    return path;
}

/**
 * Dynamically allocate and parse arguments
 * Demonstrates: dynamic array allocation
 */
char** allocate_args(int count) {
    if (count <= 0) return NULL;
    
    char **args = (char**)malloc((count + 1) * sizeof(char*));
    if (!args) {
        fprintf(stderr, "Memory allocation failed for argument array\n");
        return NULL;
    }
    
    // Initialize to NULL for safety
    for (int i = 0; i <= count; i++) {
        args[i] = NULL;
    }
    
    return args;
}

/**
 * Free dynamically allocated argument array
 * Demonstrates: proper deallocation of dynamic structures
 */
void free_args(char **args) {
    if (!args) return;
    
    for (int i = 0; args[i] != NULL; i++) {
        free(args[i]);
    }
    free(args);
}

/**
 * Memory usage statistics
 * Demonstrates: tracking memory usage
 */
void print_memory_stats(void) {
    printf("\n=== Memory Statistics ===\n");
    printf("Commands in history: %d\n", history_count);
    printf("Max history size: %d\n", MAX_HISTORY);
    
    // Calculate approximate memory usage
    size_t history_mem = 0;
    HistoryNode *current = history_head;
    while (current) {
        history_mem += sizeof(HistoryNode);
        history_mem += strlen(current->command) + 1;
        current = current->next;
    }
    
    printf("Approximate history memory: %zu bytes\n", history_mem);
    printf("=========================\n\n");
}
