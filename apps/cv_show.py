#!/usr/bin/env python3
"""
VisionOS - Computer Vision Show Command
Displays an image using OpenCV
Usage: cv-show [image_path]
"""

import sys
import cv2
import argparse
from cv_utils import read_image

def main():
    parser = argparse.ArgumentParser(description="Display an image.")
    parser.add_argument("input_path", nargs='?', help="Path to input image (optional, defaults to stdin)")
    
    args = parser.parse_args()

    img = read_image(args.input_path)
    
    if img is None:
        sys.stderr.write("Error: No input image provided or could not read image.\n")
        sys.exit(1)

    # Get image dimensions
    height, width = img.shape[:2]
    
    # Display image information
    source_name = args.input_path if args.input_path else "stdin"
    print(f"Image Source: {source_name}")
    print(f"Dimensions: {width}x{height}")
    print(f"Channels: {img.shape[2] if len(img.shape) > 2 else 1}")
    print("\nDisplaying image... Press any key to close.")
    
    # Display the image
    cv2.imshow(f"VisionOS - {source_name}", img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()
    
    return 0

if __name__ == "__main__":
    main()
