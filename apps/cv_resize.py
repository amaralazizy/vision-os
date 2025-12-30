#!/usr/bin/env python3
import sys
import cv2
import argparse
from cv_utils import read_image, write_image

def main():
    parser = argparse.ArgumentParser(description="Resize an image using bilinear interpolation.")
    parser.add_argument("input_path", nargs='?', help="Path to input image (optional, defaults to stdin)")
    parser.add_argument("--output", "-o", help="Path to save output (optional, defaults to stdout)")
    
    # Dimension arguments
    parser.add_argument("--width", "-W", type=int, help="Target width")
    parser.add_argument("--height", "-H", type=int, help="Target height")
    
    # Scale arguments
    parser.add_argument("--scale-x", "-sx", type=float, help="Scale factor for X axis")
    parser.add_argument("--scale-y", "-sy", type=float, help="Scale factor for Y axis")
    
    parser.add_argument("--no-aspect-preservation", action="store_true", 
                       help="Do not preserve aspect ratio when only one dimension/scale is provided")
    
    args = parser.parse_args()

    # Read image
    img = read_image(args.input_path)
    if img is None:
        sys.stderr.write("Error: No input image provided or could not read image.\n")
        sys.exit(1)

    # Get original dimensions
    h, w = img.shape[:2]
    new_w, new_h = w, h

    # Determine resize mode
    if args.width is not None or args.height is not None:
        new_w = args.width if args.width is not None else w
        new_h = args.height if args.height is not None else h
        
        if not args.no_aspect_preservation:
            if args.width is not None and args.height is None:
                new_h = int(h * (args.width / w))
            elif args.height is not None and args.width is None:
                new_w = int(w * (args.height / h))
            
        resized_img = cv2.resize(img, (new_w, new_h), interpolation=cv2.INTER_LINEAR)
        
    elif args.scale_x is not None or args.scale_y is not None:
        sx = args.scale_x if args.scale_x is not None else 1.0
        sy = args.scale_y if args.scale_y is not None else 1.0
        
        if not args.no_aspect_preservation:
            if args.scale_x is not None and args.scale_y is None:
                sy = sx
            elif args.scale_y is not None and args.scale_x is None:
                sx = sy
        
        resized_img = cv2.resize(img, None, fx=sx, fy=sy, interpolation=cv2.INTER_LINEAR)
        
    else:
        resized_img = img

    write_image(resized_img, args.output)

if __name__ == "__main__":
    main()
