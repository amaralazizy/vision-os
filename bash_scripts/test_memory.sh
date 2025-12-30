#!/bin/bash

# VisionOS Memory Management Test Script
# This script tests memory management features

echo "======================================"
echo "VisionOS Memory Management Test"
echo "======================================"
echo ""

echo "Test 1: Basic Command History"
echo "------------------------------"
echo -e "ls\npwd\nhistory\nexit" | ./visionos
echo ""

echo "Test 2: Memory Statistics"
echo "-------------------------"
echo -e "echo test1\necho test2\necho test3\nmem-stats\nexit" | ./visionos
echo ""

echo "Test 3: Clear History"
echo "---------------------"
echo -e "ls\npwd\necho hello\nhistory\nclear-history\nhistory\nmem-stats\nexit" | ./visionos
echo ""

echo "Test 4: History Limit (adding many commands)"
echo "---------------------------------------------"
# Generate 15 commands
commands="echo 1\necho 2\necho 3\necho 4\necho 5\necho 6\necho 7\necho 8\necho 9\necho 10\necho 11\necho 12\necho 13\necho 14\necho 15\nmem-stats\nhistory\nexit"
echo -e "$commands" | ./visionos | tail -20
echo ""

echo "======================================"
echo "All Memory Management Tests Complete!"
echo "======================================"
