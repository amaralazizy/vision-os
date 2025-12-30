import sys
import argparse
import cv2
import numpy as np
from cv_utils import read_image, write_image


def adjust_hsv(img, hue_shift=0, sat_scale=1.0, val_scale=1.0):

    # Convert BGR → HSV
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV).astype(np.float32)

    # Split channels
    h, s, v = cv2.split(hsv)

    # Hue shift
    h = h + hue_shift

    # Scale saturation and value
    s *= sat_scale
    v *= val_scale

    # Merge and convert back
    hsv_adjusted = cv2.merge([h, s, v]).astype(np.uint8)
    out = cv2.cvtColor(hsv_adjusted, cv2.COLOR_HSV2BGR)

    return out


def main():
    parser = argparse.ArgumentParser(description="Edit image HSV parameters")
    parser.add_argument("image_path", help="Path to input image")
    parser.add_argument("--h", type=int, default=0,help="Hue shift in degrees (default: 0)")
    parser.add_argument("--s", type=float, default=1.0, help="Saturation scale (default: 1.0)")
    parser.add_argument("--v", type=float, default=1.0, help="Value / brightness scale (default: 1.0)")
    parser.add_argument("-o", "--output", required=True,help="Output image path")
    args = parser.parse_args()

    img = read_image(args.image_path)
    if img is None:
        sys.stderr.write("Error: Could not read image\n")
        sys.exit(1)

    out = adjust_hsv(
        img,
        hue_shift=args.h,
        sat_scale=args.s,
        val_scale=args.v
    )

    write_image(out, args.output)


if __name__ == "__main__":
    main()
