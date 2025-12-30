# Input Validation - Quick Reference

## Validation Functions Quick Guide

| Function | Purpose | Returns |
|----------|---------|---------|
| `validate_string_not_empty(str)` | Check not NULL/empty | 1=valid, 0=invalid |
| `validate_string_length(str, max)` | Check length bounds | 1=valid, 0=invalid |
| `validate_buffer(buf, size)` | Check buffer validity | 1=valid, 0=invalid |
| `validate_args(args)` | Check arg array | 1=valid, 0=invalid |
| `validate_command_name(cmd)` | Prevent injection | 1=valid, 0=invalid |
| `validate_file_path(path)` | Check path safety | 1=valid, 0=invalid |
| `validate_int_range(val, min, max)` | Check range | 1=valid, 0=invalid |
| `validate_parse_int(str, *result)` | Safe parse | 1=valid, 0=invalid |
| `validate_pipe_count(count)` | Limit pipes | 1=valid, 0=invalid |
| `validate_fd(fd)` | Check fd >= 0 | 1=valid, 0=invalid |
| `validate_pid(pid)` | Check pid >= 0 | 1=valid, 0=invalid |
| `sanitize_input(str)` | Remove control chars | void |
| `validate_input_command(input)` | Full input check | 1=valid, 0=invalid |
| `validate_malloc(size, context)` | Safe allocation | ptr or NULL |
| `validate_array_index(idx, size)` | Bounds check | 1=valid, 0=invalid |
| `validate_redirect_type(type)` | Check redirect | 1=valid, 0=invalid |
| `validate_env_var_name(name)` | Check var name | 1=valid, 0=invalid |

## Validation Limits

```c
SHELL_MAX_INPUT      1024     // Max input length
MAX_ARGS             64       // Max arguments
MAX_PIPES            10       // Max pipes in command
MAX_COMMAND_LENGTH   256      // Max command name
MAX_ARG_LENGTH       512      // Max single argument
MAX_PATH_LENGTH      4096     // Max file path
MAX_BUFFER_SIZE      65536    // 64KB buffer max
MAX_ALLOCATION_SIZE  10MB     // Max single malloc
MAX_HISTORY          100      // Max history entries
```

## Common Usage Patterns

### Before Executing Command
```c
if (!validate_args(args)) {
    fprintf(stderr, "Error: Invalid arguments\n");
    return;
}
if (!validate_command_name(args[0])) {
    fprintf(stderr, "Error: Invalid command\n");
    return;
}
```

### Before File Operation
```c
if (!validate_file_path(path)) {
    fprintf(stderr, "Error: Invalid path\n");
    return;
}
```

### Before Memory Allocation
```c
ptr = validate_malloc(size, "operation_name");
if (!ptr) {
    return NULL;
}
```

### Before System Call
```c
if (!validate_fd(fd)) {
    perror("Invalid fd");
    return;
}
```

### Processing User Input
```c
sanitize_input(input);
if (!validate_input_command(input)) {
    fprintf(stderr, "Invalid input\n");
    continue;
}
```

## Blocked Characters in Commands

`;` `|` `&` `$` `(` `)` `<` `>` `[` `]` `{` `}` `*` `?` `!` `` ` `` `\` `"` `'`

## Where Validation is Applied

- **kernel.c**: Input, pipes, fds, pids
- **executor.c**: Commands, args, paths, redirects
- **memory.c**: Strings, allocations, ranges
- **utils.c**: Buffers, tokens
- **All operations**: Before execution

## Error Message Format

```
Validation error: [specific issue]
Error: [user-friendly message]
Warning: [informational message]
```

## Testing Validation

```bash
# Test script
./test_validation.sh

# Manual tests
echo "ls" | ./visionos                    # ✓ Valid
echo "$(echo test)" | ./visionos         # ✗ Blocked
echo "[1025 chars]" | ./visionos         # ✗ Too long
```

## Quick Checklist

When adding new code, validate:
- [ ] User input strings
- [ ] Command/file paths
- [ ] Buffer sizes
- [ ] Array indices
- [ ] Integer ranges
- [ ] System call results (fd, pid)
- [ ] Memory allocations

## See Full Documentation

- `INPUT_VALIDATION.md` - Complete guide
- `VALIDATION_SUMMARY.md` - Implementation summary
- `src/validation.c` - Function implementations
