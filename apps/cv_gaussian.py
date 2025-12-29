#!/usr/bin/env python3
import sys
import cv2
import argparse
from cv_utils import read_image, write_image

def main():
    parser = argparse.ArgumentParser(description="Apply Gaussian blur to an image.")
    parser.add_argument("input_path", nargs='?', help="Path to input image (optional, defaults to stdin)")
    parser.add_argument("--output", "-o", help="Path to save output (optional, defaults to stdout)")
    parser.add_argument("--kernel", "-k", type=int, default=5, help="Kernel size (must be odd, default: 5)")
    parser.add_argument("--sigma", "-s", type=float, default=0, help="Gaussian kernel standard deviation (default: 0)")
    
    args = parser.parse_args()

    # Validate kernel size
    if args.kernel % 2 == 0:
        sys.stderr.write("Warning: Kernel size must be odd. Adding 1.\n")
        args.kernel += 1

    img = read_image(args.input_path)
    
    if img is None:
        sys.stderr.write("Error: No input image provided.\n")
        sys.exit(1)

    # Apply Gaussian Blur
    blurred = cv2.GaussianBlur(img, (args.kernel, args.kernel), args.sigma)

    write_image(blurred, args.output)

if __name__ == "__main__":
    main()
