# Memory Management in VisionOS

## Overview

VisionOS implements comprehensive memory management to demonstrate proper allocation, tracking, and deallocation of dynamic memory in a C-based shell environment.

## Key Concepts Implemented

### 1. **Dynamic Memory Allocation**

Using `malloc()` and `strdup()` to allocate memory at runtime for:

- Command history storage
- String duplication
- Dynamic path building
- Argument arrays

### 2. **Memory Deallocation**

Proper use of `free()` to prevent memory leaks:

- Freeing individual history entries
- Cleaning up linked list nodes
- Releasing dynamically allocated strings

### 3. **Resource Tracking**

- Command history with bounded size (MAX_HISTORY = 100)
- Automatic removal of oldest entries when limit reached
- Memory usage statistics tracking

### 4. **Safe Memory Operations**

- Wrapper functions for safer allocation
- NULL pointer checks
- Error handling for allocation failures

## Implementation Details

### Data Structures

#### History Node (Linked List)

```c
typedef struct HistoryNode {
    char *command;              // Dynamically allocated command string
    struct HistoryNode *next;   // Pointer to next node
} HistoryNode;
```

### Core Functions

#### 1. `add_to_history()`

- **Purpose**: Stores commands in a linked list
- **Memory Operations**:
  - `malloc()` for node structure
  - `strdup()` for command string
- **Features**:
  - Automatic limit enforcement
  - Null checks and error handling

#### 2. `remove_oldest_history()`

- **Purpose**: Removes oldest command when limit reached
- **Memory Operations**:
  - `free()` command string
  - `free()` node structure
- **Prevents**: Unbounded memory growth

#### 3. `clear_history()`

- **Purpose**: Cleanup all allocated memory
- **Memory Operations**:
  - Iterates through linked list
  - Frees each command string and node
- **Called**: On shell exit for proper cleanup

#### 4. `safe_strdup()`

- **Purpose**: Safer string duplication with error handling
- **Features**:
  - NULL check on input
  - Allocation failure detection
  - Error messages

#### 5. `build_path()`

- **Purpose**: Dynamically construct file paths
- **Memory Operations**:
  - Calculate required size
  - Allocate exact amount needed
  - Build string with `snprintf()`
- **Note**: Caller must free returned pointer

#### 6. `allocate_args()` / `free_args()`

- **Purpose**: Dynamic argument array management
- **Features**:
  - Allocates array of string pointers
  - NULL initialization for safety
  - Proper deallocation of all elements

#### 7. `print_memory_stats()`

- **Purpose**: Display memory usage information
- **Shows**:
  - Number of commands in history
  - Maximum history capacity
  - Approximate memory usage

## Usage Examples

### Command History

```bash
visionos> ls -la
visionos> pwd
visionos> cv-show image.jpg
visionos> history
Command History:
================
   1  ls -la
   2  pwd
   3  cv-show image.jpg
```

### Memory Statistics

```bash
visionos> mem-stats
=== Memory Statistics ===
Commands in history: 15
Max history size: 100
Approximate history memory: 342 bytes
=========================
```

### Clear History

```bash
visionos> clear-history
History cleared.
```

### Automatic Cleanup on Exit

```bash
visionos> exit
Cleaning up and exiting...
```

## Memory Management Best Practices Demonstrated

### 1. **Always Check Allocation Results**

```c
HistoryNode *new_node = (HistoryNode*)malloc(sizeof(HistoryNode));
if (!new_node) {
    fprintf(stderr, "Memory allocation failed\n");
    return;
}
```

### 2. **Free What You Allocate**

```c
free(old_head->command);  // Free string first
free(old_head);           // Then free struct
```

### 3. **Set Pointers to NULL After Freeing**

```c
history_head = NULL;
history_tail = NULL;
```

### 4. **Bounded Resource Usage**

```c
if (history_count > MAX_HISTORY) {
    remove_oldest_history();
}
```

### 5. **Calculate Sizes Correctly**

```c
size_t size = strlen(dir) + 1 + strlen(file) + 1;  // dir + "/" + file + '\0'
```

## Memory Leak Prevention

The implementation prevents memory leaks through:

1. **Exit Cleanup**: `clear_history()` called before exit
2. **Bounded Growth**: Automatic removal of oldest entries
3. **Proper Deallocation**: Free in reverse order of allocation
4. **Error Handling**: Cleanup on allocation failures

## Testing Memory Management

### Using Valgrind (Memory Leak Detector)

```bash
# Compile with debug symbols
gcc -g -Wall -o visionos src/*.c

# Run with Valgrind
valgrind --leak-check=full --show-leak-kinds=all ./visionos

# Example session:
visionos> ls
visionos> history
visionos> mem-stats
visionos> exit

# Valgrind should report: "All heap blocks were freed -- no leaks are possible"
```

### Manual Testing

```bash
# Test history limit
visionos> history        # Should show recent commands
visionos> clear-history  # Should free all memory
visionos> mem-stats      # Should show 0 commands
visionos> exit           # Should cleanup properly
```

## File Structure

```
src/
├── memory.c         # Memory management implementation
├── kernel.c         # Main shell (uses memory functions)
├── utils.c          # Utility functions
├── executor.c       # Command execution
└── visionos.h       # Header with memory function declarations
```

## Integration with Shell

The memory management is seamlessly integrated:

1. **Every Command**: Added to history via `add_to_history()`
2. **Built-in Commands**:
   - `history` - Show command history
   - `clear-history` - Free all history memory
   - `mem-stats` - Display memory statistics
3. **Exit**: Automatic cleanup via `clear_history()`

## Educational Value

This implementation demonstrates:

- ✓ Dynamic memory allocation with `malloc()`
- ✓ String management with `strdup()`
- ✓ Linked list operations
- ✓ Memory deallocation with `free()`
- ✓ Prevention of memory leaks
- ✓ Bounded resource usage
- ✓ Error handling for allocation failures
- ✓ Memory usage tracking
- ✓ Safe memory operations

## Advanced Features

### Automatic History Management

- Commands automatically stored
- Oldest removed when limit reached
- No manual intervention needed

### Memory Efficiency

- Only stores command strings (not full parse trees)
- Bounded size prevents unlimited growth
- Efficient linked list structure

### Debug Support

- Memory statistics for monitoring
- Clear error messages on failures
- Easy to test with Valgrind

## Conclusion

The memory management implementation in VisionOS provides a practical example of proper dynamic memory handling in systems programming, demonstrating allocation, tracking, deallocation, and resource management in a real-world shell application.
