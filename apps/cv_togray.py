#!/usr/bin/env python3
import sys
import cv2
import argparse
from cv_utils import read_image, write_image

def main():
    parser = argparse.ArgumentParser(description="Convert image to grayscale.")
    parser.add_argument("input_path", nargs='?', help="Path to input image (optional, defaults to stdin)")
    parser.add_argument("--output", "-o", help="Path to save output (optional, defaults to stdout)")
    
    args = parser.parse_args()

    # Try to read from file if provided, else stdin
    img = read_image(args.input_path)
    
    if img is None:
        sys.stderr.write("Error: No input image provided.\n")
        sys.exit(1)

    # Convert to grayscale
    if len(img.shape) == 3:
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    else:
        gray = img

    write_image(gray, args.output)

if __name__ == "__main__":
    main()
