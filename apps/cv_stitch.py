import sys
import cv2
import numpy as np
import argparse
import os
from cv_match import compute_matches, RANSAC_filter, keypoints_to_array, matches_to_array
from cv_utils import read_image, write_image

def main():
    parser = argparse.ArgumentParser(description="Stitch two images into a panorama using SIFT matches.")
    parser.add_argument("image1_path", help="Path to the first input image")
    parser.add_argument("image2_path", help="Path to the second input image")
    parser.add_argument("--out_dir", default="cv_stitch_out", help="Directory to save output files")
    parser.add_argument("--out_image", default="panorama.png", help="Output panorama filename")
    parser.add_argument("--lowe_ratio", "-r", type=float, default=0.75, help="Lowe's ratio for matching")
    parser.add_argument("--reproj_thresh", "-t", type=float, default=5.0, help="RANSAC reprojection threshold")
    args = parser.parse_args()

    # Create output directory if it doesn't exist
    os.makedirs(args.out_dir, exist_ok=True)

    # Read input images
    img1 = read_image(args.image1_path)
    img2 = read_image(args.image2_path)

    if img1 is None or img2 is None:
        sys.stderr.write("Error: One or both input images could not be read.\n")
        sys.exit(1)

    # Compute matches
    kp1, kp2, des1, des2, good_matches = compute_matches(img1, img2, lowe_ratio=args.lowe_ratio)

    # Use RANSAC to filter matches
    matches_mask = RANSAC_filter(kp1, kp2, good_matches, reproj_thresh=args.reproj_thresh)

    # Validate sufficient inliers
    if matches_mask is None or sum(matches_mask) < 4:
        sys.stderr.write("Error: Not enough inlier matches for homography.\n")
        sys.exit(1)

    # Compute homography using inlier matches
    src_pts = np.float32(
        [kp1[m.queryIdx].pt for i, m in enumerate(good_matches) if matches_mask[i]]
    ).reshape(-1, 1, 2)
    dst_pts = np.float32(
        [kp2[m.trainIdx].pt for i, m in enumerate(good_matches) if matches_mask[i]]
    ).reshape(-1, 1, 2)

    H, _ = cv2.findHomography(src_pts, dst_pts, 0)  # already filtered by RANSAC

    # Calculate size of panorama
    width = img1.shape[1] + img2.shape[1]
    height = max(img1.shape[0], img2.shape[0])

    # Warp images to create panorama
    panorama = cv2.warpPerspective(img1, H, (width, height))
    panorama[0:img2.shape[0], 0:img2.shape[1]] = img2

    # Convert to grayscale and threshold to find non-black areas
    gray = cv2.cvtColor(panorama, cv2.COLOR_BGR2GRAY)
    _, thresh = cv2.threshold(gray, 1, 255, cv2.THRESH_BINARY)

    # Find bounding rectangle of non-black area
    x, y, w, h = cv2.boundingRect(thresh)
    panorama_cropped = panorama[y:y+h, x:x+w]


    # Save the resulting panorama
    out_path = os.path.join(args.out_dir, args.out_image)
    write_image(panorama_cropped, out_path)
    print(f"Panorama saved to {out_path}")

if __name__ == "__main__":
    main()
