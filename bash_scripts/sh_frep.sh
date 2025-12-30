#!/usr/bin/env bash

# sh-find: enhanced find + grep

# Check for correct number of arguments
if [[ $# -lt 2 ]]; then
    echo "Usage: sh-find <dir> <pattern>"
    exit 1
fi

DIR="$1"
PATTERN="$2"

# Check if directory exists
if [[ ! -d "$DIR" ]]; then
    echo "Error: $DIR is not a directory"
    exit 1
fi

matches=()   # Bash array

# Iterate over files in the directory and search for the pattern
for file in "$DIR"/*; do
    if [[ -f "$file" ]]; then
        if grep -q "$PATTERN" "$file"; then
            matches+=("$file")
        fi
    fi
done

# Output results in a user-friendly format
echo "Matches:"
for m in "${matches[@]}"; do
    echo " - $m"
done
