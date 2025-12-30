# Input Validation in VisionOS

## Overview

VisionOS implements comprehensive input validation to ensure security, stability, and prevent common vulnerabilities in shell operations. All user inputs, system calls, and memory operations are validated before execution.

## Validation Categories

### 1. String Validation

#### `validate_string_not_empty()`
- **Purpose**: Ensures strings are not NULL or empty
- **Checks**: NULL pointer, zero length
- **Used in**: All string operations

#### `validate_string_length()`
- **Purpose**: Validates string length within bounds
- **Checks**: String not empty, length <= max_len
- **Limits**: 
  - Commands: 256 chars
  - Arguments: 512 chars  
  - Paths: 4096 chars
  - Input: 1024 chars

#### `sanitize_input()`
- **Purpose**: Removes dangerous control characters
- **Action**: Replaces non-printable chars (except tab/newline) with spaces
- **Applied to**: All user input

### 2. Buffer Validation

#### `validate_buffer()`
- **Purpose**: Validates buffer pointer and size
- **Checks**:
  - Buffer not NULL
  - Size > 0
  - Size <= MAX_BUFFER_SIZE (64KB)
- **Used in**: Memory operations, path operations

### 3. Command Validation

#### `validate_command_name()`
- **Purpose**: Prevents command injection attacks
- **Blocks**: Dangerous characters: `;|&$()<>[]{}*?!\`\\"'`
- **Allows**: Alphanumeric, dash, underscore
- **Used in**: All command execution paths

#### `validate_args()`
- **Purpose**: Validates argument array structure
- **Checks**:
  - Array not NULL
  - First element (command) exists
  - Each argument valid length
  - Argument count <= MAX_ARGS (64)
- **Used in**: Command execution, piping

#### `validate_input_command()`
- **Purpose**: Comprehensive input validation
- **Checks**:
  - Not empty
  - Length within limits
  - No non-printable characters
  - No excessive special character sequences (max 3 consecutive)
- **Applied to**: Every command entered

### 4. File Path Validation

#### `validate_file_path()`
- **Purpose**: Prevents path traversal and injection attacks
- **Checks**:
  - Not empty
  - Length <= MAX_PATH_LENGTH
  - No null bytes
  - Warns on ".." patterns
- **Used in**: File operations, redirections, script execution

### 5. Numeric Validation

#### `validate_int_range()`
- **Purpose**: Ensures integers within acceptable range
- **Checks**: min <= value <= max
- **Used in**: Array indices, counts, limits

#### `validate_parse_int()`
- **Purpose**: Safely parse string to integer
- **Checks**:
  - Valid number format
  - No overflow/underflow
  - No trailing characters
  - Within INT_MIN to INT_MAX
- **Returns**: Parsed value via pointer
- **Used in**: Argument parsing

### 6. System Resource Validation

#### `validate_fd()`
- **Purpose**: Validates file descriptors
- **Checks**: fd >= 0
- **Used in**: Pipe operations, redirections

#### `validate_pid()`
- **Purpose**: Validates process IDs
- **Checks**: pid >= 0
- **Used in**: Fork operations, process management

#### `validate_pipe_count()`
- **Purpose**: Limits number of pipes
- **Checks**: 0 <= count <= MAX_PIPES (10)
- **Prevents**: Resource exhaustion

### 7. Memory Validation

#### `validate_malloc()`
- **Purpose**: Safe memory allocation wrapper
- **Checks**:
  - Size > 0
  - Size <= MAX_ALLOCATION_SIZE (10MB)
  - Allocation succeeded
- **Returns**: NULL on failure with error message
- **Used in**: All dynamic allocations

#### `validate_array_index()`
- **Purpose**: Prevents buffer overflow
- **Checks**: 0 <= index < array_size
- **Used in**: Array access operations

### 8. Type-Specific Validation

#### `validate_redirect_type()`
- **Purpose**: Validates redirection operators
- **Checks**: Type in valid range (REDIRECT_NONE to REDIRECT_INPUT)
- **Used in**: I/O redirection

#### `validate_env_var_name()`
- **Purpose**: Validates environment variable names
- **Checks**:
  - Starts with letter or underscore
  - Contains only alphanumeric and underscore
- **Used in**: Environment variable operations

## Integration Points

### In `kernel.c` (Main Loop)
```c
// Input validation
if (!validate_string_length(input, SHELL_MAX_INPUT)) {
    fprintf(stderr, "Error: Input too long\n");
    continue;
}

if (!validate_input_command(input)) {
    fprintf(stderr, "Error: Invalid input\n");
    continue;
}

// Pipe count validation
if (!validate_pipe_count(num_cmds - 1)) {
    fprintf(stderr, "Error: Too many pipes\n");
    continue;
}

// File descriptor validation
if (!validate_fd(pipefd[0]) || !validate_fd(pipefd[1])) {
    fprintf(stderr, "Error: Invalid pipe file descriptors\n");
    break;
}

// Process ID validation
if (!validate_pid(pid)) {
    perror("fork failed");
    break;
}
```

### In `executor.c` (Command Execution)
```c
// Argument validation
if (!validate_args(args)) {
    fprintf(stderr, "Error: Invalid arguments\n");
    exit(1);
}

// Command name validation
if (!validate_command_name(cv_cmd)) {
    fprintf(stderr, "Error: Invalid CV command name\n");
    exit(1);
}

// Path validation
if (!validate_file_path(script_path)) {
    fprintf(stderr, "Error: Invalid script path\n");
    exit(1);
}

// File descriptor validation in redirection
if (validate_fd(fd)) {
    dup2(fd, STDOUT_FILENO);
}
```

### In `memory.c` (Memory Management)
```c
// String validation before duplication
if (!validate_string_not_empty(str)) {
    return NULL;
}

// Size validation before allocation
char **args = (char**)validate_malloc(alloc_size, "argument array");

// Range validation
if (!validate_int_range(count, 1, MAX_ARGS)) {
    fprintf(stderr, "Error: Invalid argument count\n");
    return NULL;
}
```

### In `utils.c` (Utilities)
```c
// Buffer validation
if (!validate_buffer(buffer, size)) {
    fprintf(stderr, "Error: Invalid buffer\n");
    return;
}

// Input sanitization
sanitize_input(input);

// Token validation
if (validate_string_length(token, MAX_ARG_LENGTH)) {
    args[argc++] = token;
}
```

## Security Benefits

### 1. **Command Injection Prevention**
- Blocks dangerous characters in command names
- Validates all user input before execution
- Sanitizes control characters

### 2. **Buffer Overflow Prevention**
- Validates all buffer sizes
- Checks array indices
- Enforces length limits on strings

### 3. **Path Traversal Prevention**
- Validates file paths
- Warns on suspicious patterns
- Checks for null bytes

### 4. **Resource Exhaustion Prevention**
- Limits number of pipes
- Limits allocation sizes
- Bounds history size

### 5. **Integer Overflow Prevention**
- Validates numeric conversions
- Checks for overflow/underflow
- Enforces range limits

## Error Handling

All validation functions:
- Return 0/NULL on failure, 1/pointer on success
- Print descriptive error messages to stderr
- Provide context (function name, expected values)
- Allow graceful error recovery

Example error messages:
```
Validation error: NULL string
Validation error: String length 2048 exceeds maximum 1024
Validation error: Command contains dangerous character ';'
Validation error: Path contains null bytes
Validation error: Invalid file descriptor -1
Validation error: Value 150 out of range [0, 100]
```

## Performance Considerations

### Minimal Overhead
- Most validations are simple comparisons (O(1))
- String validation is O(n) but on small strings
- No complex regex or parsing
- Validation failures caught early

### Optimizations
- NULL checks before expensive operations
- Early returns on obvious failures
- Inline candidate functions for hot paths
- Minimal memory allocation in validators

## Testing Input Validation

### Valid Inputs
```bash
visionos> ls -la
visionos> cv-show image.jpg
visionos> echo "Hello World"
visionos> cat file.txt | grep pattern
```

### Invalid Inputs (Blocked)
```bash
visionos> ; rm -rf /          # Dangerous character
visionos> $(malicious)        # Command injection attempt
visionos> ||||||||||||        # Too many consecutive special chars
visionos> [string > 1024]     # Input too long
```

### Security Tests
```bash
# Path traversal attempts
visionos> cv-show ../../../../etc/passwd   # Warned but processed

# Null byte injection
visionos> cv-show file.txt\0malicious      # Blocked

# Integer overflow
visionos> [operation with out-of-range numbers]  # Blocked
```

## Configuration

Validation limits defined in `visionos.h`:
```c
#define SHELL_MAX_INPUT 1024
#define MAX_ARGS 64
#define MAX_PIPES 10
#define MAX_COMMAND_LENGTH 256
#define MAX_ARG_LENGTH 512
#define MAX_PATH_LENGTH 4096
#define MAX_BUFFER_SIZE 65536
#define MAX_ALLOCATION_SIZE (1024 * 1024 * 10)  // 10MB
```

Adjust these values based on security requirements and system resources.

## Best Practices

1. **Validate Early**: Check inputs at entry points
2. **Fail Safely**: Return error rather than crash
3. **Be Specific**: Clear error messages help debugging
4. **Layer Defense**: Multiple validation layers
5. **Sanitize Always**: Clean input before processing
6. **Bound Resources**: Limit all allocations and operations
7. **Check Returns**: Validate all system call results

## Future Enhancements

Potential additions:
- Whitelist-based command validation
- Rate limiting for repeated failures
- Logging of validation failures
- Stricter path validation modes
- Custom validation rules per command
- Security audit mode with detailed logging

## Conclusion

VisionOS's comprehensive input validation provides multiple layers of defense against common security vulnerabilities while maintaining usability and performance. Every input is validated, every allocation is checked, and every operation is bounded.
