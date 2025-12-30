#!/usr/bin/env python3
import sys
import cv2
import numpy as np
import argparse
from cv_utils import read_image, write_image

def main():
    parser = argparse.ArgumentParser(description="Apply median filter to an image.")
    parser.add_argument("input_path", nargs='?', help="Path to input image (optional, defaults to stdin)")
    parser.add_argument("--output", "-o", help="Path to save output (optional, defaults to stdout)")
    
    # Median filter parameter
    parser.add_argument("--ksize", "-k", type=int, default=3,
                        help="Kernel size for median filter (must be odd, default: 3)")
    
    args = parser.parse_args()

    # Check that kernel size is odd
    if args.ksize % 2 == 0:
        sys.stderr.write("Error: Kernel size must be an odd number.\n")
        sys.exit(1)

    # Read image
    img = read_image(args.input_path)
    if img is None:
        sys.stderr.write("Error: No input image provided.\n")
        sys.exit(1)

    # Apply median filter
    filtered = cv2.medianBlur(img, args.ksize)

    # Save or output image
    write_image(filtered, args.output)

if __name__ == "__main__":
    main()
