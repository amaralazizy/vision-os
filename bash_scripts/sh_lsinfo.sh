#!/usr/bin/env bash

# sh-lsinfo: List files with detailed information

# Check for correct number of arguments
DIR="${1:-.}"  # Default to current directory if no argument

# Check if directory exists
if [[ ! -d "$DIR" ]]; then
    echo "Error: $DIR is not a directory"
    exit 1
fi

echo "Listing files in directory: $DIR"
echo "------------------------------------"

# Iterate over files
for file in "$DIR"/*; do
    if [[ -e "$file" ]]; then
        perms=$(stat -c "%A" "$file")
        size=$(stat -c "%s" "$file")
        modtime=$(stat -c "%y" "$file")
        echo "$perms | $size bytes | $modtime | $(basename "$file")"
    fi
done
echo "------------------------------------"