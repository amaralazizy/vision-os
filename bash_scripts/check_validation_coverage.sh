#!/bin/bash

# VisionOS Validation Coverage Report
# Shows where each validation function is used

echo "======================================"
echo "Input Validation Coverage Report"
echo "======================================"
echo ""

echo "Searching for validation function usage across source files..."
echo ""

VALIDATION_FUNCS=(
    "validate_string_not_empty"
    "validate_string_length"
    "validate_buffer"
    "validate_args"
    "validate_command_name"
    "validate_file_path"
    "validate_int_range"
    "validate_parse_int"
    "validate_pipe_count"
    "validate_fd"
    "validate_pid"
    "sanitize_input"
    "validate_input_command"
    "validate_malloc"
    "validate_array_index"
    "validate_redirect_type"
    "validate_env_var_name"
)

for func in "${VALIDATION_FUNCS[@]}"; do
    echo "Function: $func"
    echo "----------------------------------------"
    grep -n "$func" src/*.c 2>/dev/null | sed 's/^/  /' || echo "  Not used"
    echo ""
done

echo "======================================"
echo "Summary Statistics"
echo "======================================"
echo ""

total_validations=$(grep -c "validate_" src/*.c 2>/dev/null | awk -F: '{sum+=$2} END {print sum}')
echo "Total validation calls: $total_validations"
echo ""

for file in src/*.c; do
    count=$(grep -c "validate_" "$file" 2>/dev/null || echo 0)
    if [ "$count" -gt 0 ]; then
        echo "  $(basename $file): $count validation calls"
    fi
done
