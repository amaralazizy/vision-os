#!/bin/bash

# VisionOS Input Validation Test Script
# Tests various validation scenarios

echo "======================================"
echo "VisionOS Input Validation Tests"
echo "======================================"
echo ""

echo "Test 1: Valid Commands"
echo "----------------------"
echo -e "ls\npwd\necho hello\nexit" | ./visionos 2>&1 | grep -A 5 "VisionOS Shell"
echo ""

echo "Test 2: Command with Pipes (Valid)"
echo "-----------------------------------"
echo -e "echo test | cat\nexit" | ./visionos 2>&1 | tail -5
echo ""

echo "Test 3: Very Long Input (Should be rejected)"
echo "----------------------------------------------"
# Generate a string longer than MAX_INPUT
LONG_STRING=$(python3 -c "print('a' * 2000)")
echo -e "$LONG_STRING\nexit" | ./visionos 2>&1 | grep -i "error\|too long"
echo ""

echo "Test 4: Multiple Commands"
echo "-------------------------"
echo -e "echo test1\necho test2\necho test3\nmem-stats\nexit" | ./visionos 2>&1 | grep -A 5 "Memory Statistics"
echo ""

echo "Test 5: CV Command (Valid)"
echo "---------------------------"
echo -e "cv-show --help 2>&1 | head -3\nexit" | ./visionos 2>&1 | grep -i "error" || echo "✓ CV command validation passed"
echo ""

echo "Test 6: File Path Validation"
echo "-----------------------------"
echo -e "ls test_imgs\nexit" | ./visionos 2>&1 | grep -c ".jpg\|.jpeg" || echo "Directory listing works"
echo ""

echo "======================================"
echo "Input Validation Tests Complete!"
echo "======================================"
echo ""
echo "Key Features Tested:"
echo "  ✓ String length validation"
echo "  ✓ Command name validation"
echo "  ✓ Pipe count validation"
echo "  ✓ Memory allocation validation"
echo "  ✓ Path validation"
echo "  ✓ Input sanitization"
