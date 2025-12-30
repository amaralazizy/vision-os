#!/usr/bin/env python3
import sys
import cv2
import numpy as np
import argparse
import signal
from cv_utils import read_image, write_image

def signal_handler(sig, frame):
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

def main():
    parser = argparse.ArgumentParser(description="Apply sharpening to an image.")
    parser.add_argument("input_path", nargs='?', help="Path to input image (optional, defaults to stdin)")
    parser.add_argument("--output", "-o", help="Path to save output (optional, defaults to stdout)")
    
    # Method
    parser.add_argument("--method", "-m", choices=['kernel', 'unsharp'], 
                       default='kernel', help="Sharpening method (default: kernel)")
    
    # Parameters
    parser.add_argument("--strength", "-s", type=float, default=1.0, 
                       help="Strength/Intensity of sharpening (default: 1.0)")
    
    # Kernel specific
    parser.add_argument("--kernel-type", "-k", choices=['basic', 'strong'], default='basic',
                       help="Kernel type for 'kernel' method: basic (cross) or strong (box) (default: basic)")
    
    # Unsharp specific
    parser.add_argument("--radius", "-r", type=int, default=3,
                       help="Blur radius for unsharp mask (default: 3)")
    
    args = parser.parse_args()

    # Read image
    img = read_image(args.input_path)
    if img is None:
        sys.stderr.write("Error: No input image provided.\n")
        sys.exit(1)

    output_img = img

    if args.method == 'kernel':
        if args.kernel_type == 'basic':
            # Basic sharpening kernel (cross)
            kernel = np.array([[0, -1, 0], 
                             [-1, 5, -1], 
                             [0, -1, 0]])
        else: # strong
            # Strong sharpening kernel (box)
            kernel = np.array([[-1, -1, -1], 
                             [-1, 9, -1], 
                             [-1, -1, -1]])
        
        # Apply filter
        sharpened = cv2.filter2D(img, -1, kernel)
        
        # Apply strength blending if requested
        if args.strength != 1.0:
            # Linear interpolation: (1 - strength) * original + strength * sharpened
            # Note: strength > 1 will extrapolate, which is fine for desired "extra sharp" effect,
            # but might clip. cv2.addWeighted handles clipping.
            # Using addWeighted: src1*alpha + src2*beta + gamma
            output_img = cv2.addWeighted(img, 1.0 - args.strength, sharpened, args.strength, 0)
        else:
            output_img = sharpened

    elif args.method == 'unsharp':
        # Unsharp Masking
        # Sharpened = Original + Amount * (Original - Blurred)
        #           = Original * (1 + Amount) - Blurred * Amount
        
        # Ensure odd radius
        r = args.radius if args.radius % 2 == 1 else args.radius + 1
        
        blurred = cv2.GaussianBlur(img, (r, r), 0)
        
        output_img = cv2.addWeighted(img, 1.0 + args.strength, blurred, -args.strength, 0)

    write_image(output_img, args.output)

if __name__ == "__main__":
    main()
