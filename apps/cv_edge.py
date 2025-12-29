#!/usr/bin/env python3
import sys
import cv2
import numpy as np
import argparse
from cv_utils import read_image, write_image

def main():
    parser = argparse.ArgumentParser(description="Apply edge detection to an image.")
    parser.add_argument("input_path", nargs='?', help="Path to input image (optional, defaults to stdin)")
    parser.add_argument("--output", "-o", help="Path to save output (optional, defaults to stdout)")
    
    # Edge detection method
    parser.add_argument("--method", "-m", choices=['canny', 'sobel', 'laplacian'], 
                       default='canny', help="Edge detection method (default: canny)")
    
    # Canny parameters
    parser.add_argument("--threshold1", "-t1", type=int, default=100, 
                       help="Canny: First threshold for hysteresis (default: 100)")
    parser.add_argument("--threshold2", "-t2", type=int, default=200, 
                       help="Canny: Second threshold for hysteresis (default: 200)")
    
    # Sobel parameters
    parser.add_argument("--ksize", "-k", type=int, default=3, 
                       help="Sobel/Laplacian: Kernel size (default: 3)")
    parser.add_argument("--direction", "-d", choices=['x', 'y', 'both'], default='both',
                       help="Sobel: Gradient direction (default: both)")
    
    args = parser.parse_args()

    # Read image
    img = read_image(args.input_path)
    if img is None:
        sys.stderr.write("Error: No input image provided.\n")
        sys.exit(1)

    # Convert to grayscale if needed
    if len(img.shape) == 3:
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    else:
        gray = img

    # Apply edge detection based on method
    if args.method == 'canny':
        edges = cv2.Canny(gray, args.threshold1, args.threshold2)
        
    elif args.method == 'sobel':
        if args.direction == 'x':
            edges = cv2.Sobel(gray, cv2.CV_64F, 1, 0, ksize=args.ksize)
            edges = np.uint8(np.absolute(edges))
        elif args.direction == 'y':
            edges = cv2.Sobel(gray, cv2.CV_64F, 0, 1, ksize=args.ksize)
            edges = np.uint8(np.absolute(edges))
        else:  # both
            sobelx = cv2.Sobel(gray, cv2.CV_64F, 1, 0, ksize=args.ksize)
            sobely = cv2.Sobel(gray, cv2.CV_64F, 0, 1, ksize=args.ksize)
            edges = np.uint8(np.sqrt(sobelx**2 + sobely**2))
            
    elif args.method == 'laplacian':
        edges = cv2.Laplacian(gray, cv2.CV_64F, ksize=args.ksize)
        edges = np.uint8(np.absolute(edges))

    write_image(edges, args.output)

if __name__ == "__main__":
    main()
