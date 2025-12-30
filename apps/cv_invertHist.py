#!/usr/bin/env python3
import sys
import cv2
import numpy as np
import argparse
from cv_utils import read_image, write_image

def main():
    parser = argparse.ArgumentParser(description="Invert the histogram of an image.")
    parser.add_argument("input_path", nargs='?', help="Path to input image (optional, defaults to stdin)")
    parser.add_argument("--output", "-o", help="Path to save output (optional, defaults to stdout)")
    args = parser.parse_args()

    # Read image
    img = read_image(args.input_path)
    if img is None:
        sys.stderr.write("Error: No input image provided.\n")
        sys.exit(1)

    # Convert to grayscale if image is colored
    if len(img.shape) == 3 and img.shape[2] == 3:
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    else:
        gray = img

    # Invert histogram
    inverted = cv2.equalizeHist(gray)
    inverted = 255 - inverted  # Invert the pixel values

    # Save/output
    write_image(inverted, args.output)

if __name__ == "__main__":
    main()
