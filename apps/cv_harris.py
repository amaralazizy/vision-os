#!/usr/bin/env python3
import sys
import cv2
import numpy as np
import argparse
from cv_utils import read_image, write_image

def main():
    parser = argparse.ArgumentParser(description="Harris corner detection on an image.")
    parser.add_argument("input_path", nargs='?', help="Path to input image (optional, defaults to stdin)")
    parser.add_argument("--output", "-o", help="Path to save output (optional, defaults to stdout)")
    parser.add_argument("--blockSize", type=int, default=2, help="Neighborhood size for corner detection (default: 2)")
    parser.add_argument("--ksize", type=int, default=3, help="Aperture parameter for Sobel operator (default: 3)")
    parser.add_argument("--k", type=float, default=0.04, help="Harris detector free parameter (default: 0.04)")
    parser.add_argument("--threshold", type=float, default=0.01, help="Threshold for detecting corners (default: 0.01)")

    args = parser.parse_args()

    # Read image
    img = read_image(args.input_path)
    if img is None:
        sys.stderr.write("Error: No input image provided.\n")
        sys.exit(1)

    # Convert to grayscale if necessary
    if len(img.shape) == 3 and img.shape[2] == 3:
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    else:
        gray = img

    gray = np.float32(gray)

    # Apply Harris corner detection
    dst = cv2.cornerHarris(gray, blockSize=args.blockSize, ksize=args.ksize, k=args.k)

    # Dilate to mark corners
    dst = cv2.dilate(dst, None)

    # Create output image with corners marked in red
    output_img = cv2.cvtColor(gray.astype(np.uint8), cv2.COLOR_GRAY2BGR)
    output_img[dst > args.threshold * dst.max()] = [0, 0, 255]

    # Save/output
    write_image(output_img, args.output)

if __name__ == "__main__":
    main()
