#!/usr/bin/env python3
"""
VisionOS - Computer Vision Show Command
Displays an image using OpenCV
Usage: cv-show <image_path>
"""

import sys
import os

try:
    import cv2
except ImportError:
    print("Error: OpenCV (cv2) is not installed.")
    print("Please install it using: pip install opencv-python")
    sys.exit(1)


def show_image(image_path):
    """Display an image using OpenCV"""
    # Check if file exists
    if not os.path.exists(image_path):
        print(f"Error: Image file '{image_path}' not found.")
        return 1
    
    # Read the image
    img = cv2.imread(image_path)
    
    if img is None:
        print(f"Error: Could not read image '{image_path}'.")
        print("Supported formats: jpg, jpeg, png, bmp, etc.")
        return 1
    
    # Get image dimensions
    height, width = img.shape[:2]
    
    # Display image information
    print(f"Image: {image_path}")
    print(f"Dimensions: {width}x{height}")
    print(f"Channels: {img.shape[2] if len(img.shape) > 2 else 1}")
    print("\nDisplaying image... Press any key to close.")
    
    # Display the image
    cv2.imshow(f"VisionOS - {os.path.basename(image_path)}", img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()
    
    return 0


def main():
    """Main entry point for cv-show command"""
    if len(sys.argv) < 2:
        print("Usage: cv-show <image_path>")
        print("Example: cv-show img.jpg")
        return 1
    
    image_path = sys.argv[1]
    return show_image(image_path)


if __name__ == "__main__":
    sys.exit(main())
