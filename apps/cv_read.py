#!/usr/bin/env python3
import sys
import argparse
from cv_utils import read_image, write_image

def main():
    parser = argparse.ArgumentParser(description="Read an image and output it to the pipeline.")
    parser.add_argument("input_path", nargs='?', help="Path to the input image")
    parser.add_argument("--output", "-o", help="Path to save the output image (optional)")
    
    args = parser.parse_args()

    if not args.input_path:
        sys.stderr.write("Error: Input path is required for cv-read.\n")
        sys.exit(1)

    img = read_image(args.input_path)
    if img is None:
        sys.stderr.write(f"Error: Could not read image from {args.input_path}\n")
        sys.exit(1)

    write_image(img, args.output)

if __name__ == "__main__":
    main()
