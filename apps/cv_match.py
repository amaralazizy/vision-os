import sys
import cv2
import numpy as np
import argparse
from cv_utils import read_image, write_image

def main():
    parser = argparse.ArgumentParser(description="Match features between two images using ORB.")
    parser.add_argument("image1_path", help="Path to the first input image")
    parser.add_argument("image2_path", help="Path to the second input image")
    parser.add_argument("--output", "-o", help="Path to save output image with matches (optional, defaults to stdout)")
    parser.add_argument("--max_matches", "-m", type=int, default=0,help="Maximum number of matches to display. Default (0 = all)")
 
    args = parser.parse_args()

    img1 = read_image(args.image1_path)
    img2 = read_image(args.image2_path)
    
    if img1 is None or img2 is None:
        sys.stderr.write("Error: One or both input images could not be read.\n")
        sys.exit(1)

    # Initialize SIFT detector
    sift = cv2.SIFT_create()

    # Find the keypoints and descriptors with ORB
    kp1, des1 = sift.detectAndCompute(img1, None)
    kp2, des2 = sift.detectAndCompute(img2, None)

    if des1 is None or des2 is None:
        sys.stderr.write("Error: No descriptors found in one or both images.\n")
        sys.exit(1)


    # Create BFMatcher object
    bf = cv2.BFMatcher(cv2.NORM_L2, crossCheck=False)

    # Match descriptors using KNN algorithm
    matches = bf.knnMatch(des1, des2, k=2)

    # Apply ratio test
    good_matches = []
    for m, n in matches:
        # If the distance of the best match (m) is significantly smaller than the second best match (n), it's a good match.
        if m.distance < 0.75 * n.distance: # A common threshold is 0.75
            good_matches.append(m)

    # Use RANSAC to filter matches
    if len(good_matches) >= 4:
        src_pts = np.float32([kp1[m.queryIdx].pt for m in good_matches]).reshape(-1, 1, 2)
        dst_pts = np.float32([kp2[m.trainIdx].pt for m in good_matches]).reshape(-1, 1, 2)

        M, mask = cv2.findHomography(src_pts, dst_pts, cv2.RANSAC, 5.0)
        matches_mask = mask.ravel().tolist()
    else:
        matches_mask = None

    # Draw matches
    if args.max_matches > 0:
        draw_matches = good_matches[:args.max_matches]
        draw_mask = matches_mask[:len(draw_matches)] if matches_mask is not None else None
    else:
        draw_matches = good_matches
        draw_mask = matches_mask


    matched_img = cv2.drawMatches(
        img1, kp1, img2, kp2,
        draw_matches,
        None,
        matchColor=(0, 255, 0),
        matchesMask=draw_mask,
        flags=cv2.DrawMatchesFlags_NOT_DRAW_SINGLE_POINTS
    )


    # Write output image
    write_image(matched_img, args.output)

if __name__ == "__main__":
    main()