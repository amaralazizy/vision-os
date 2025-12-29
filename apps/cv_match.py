import sys
import cv2
import numpy as np
import argparse
from cv_utils import read_image, write_image
import os

def keypoints_to_array(kps):
    return np.array([
        (kp.pt[0], kp.pt[1], kp.size, kp.angle,
         kp.response, kp.octave, kp.class_id)
        for kp in kps
    ], dtype=np.float32)

def matches_to_array(matches):
    return np.array([
        (m.queryIdx, m.trainIdx, m.distance)
        for m in matches
    ], dtype=np.float32)

def compute_matches(img1, img2, lowe_ratio=0.75):
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
        if m.distance < lowe_ratio * n.distance: # A common threshold is 0.75
            good_matches.append(m)

    return kp1, kp2, des1, des2, good_matches

def RANSAC_filter(kp1, kp2, matches, reproj_thresh =5.0):
    if len(matches) >= 4:
        src_pts = np.float32([kp1[m.queryIdx].pt for m in matches]).reshape(-1, 1, 2)
        dst_pts = np.float32([kp2[m.trainIdx].pt for m in matches]).reshape(-1, 1, 2)

        _, mask = cv2.findHomography(src_pts, dst_pts, cv2.RANSAC, reproj_thresh)
        matches_mask = mask.ravel().tolist()
    else:
        matches_mask = None

    return matches_mask

def main():
    parser = argparse.ArgumentParser(description="Match features between two images using SIFT.")
    parser.add_argument("image1_path", help="Path to the first input image")
    parser.add_argument("image2_path", help="Path to the second input image")
    parser.add_argument("--out_dir", default="cv_match_out", help="Directory to save output files")
    parser.add_argument("--prefix", default="matches", help="Prefix for output files")
    parser.add_argument("--save_image", action="store_true", help="Save visualization image of matches")
    parser.add_argument("--max_matches", "-m", type=int, default=0, help="Maximum number of matches to draw (0 = all)")
    parser.add_argument("--lowe_ratio", "-r", type=float, default=0.75, help="Lowe's ratio threshold for filtering matches (default=0.75)")
    parser.add_argument("--reproj_thresh", "-t", type=float, default=5.0, help="RANSAC reprojection threshold in pixels (default=5.0)")

    args = parser.parse_args()

    # Create output directory if it doesn't exist
    os.makedirs(args.out_dir, exist_ok=True)

    img1 = read_image(args.image1_path)
    img2 = read_image(args.image2_path)
    
    if img1 is None or img2 is None:
        sys.stderr.write("Error: One or both input images could not be read.\n")
        sys.exit(1)

    # Compute matches
    kp1, kp2, des1, des2, good_matches = compute_matches(img1, img2, lowe_ratio=args.lowe_ratio)

    # Use RANSAC to filter matches
    matches_mask = RANSAC_filter(kp1, kp2, good_matches, reproj_thresh=args.reproj_thresh)

    # Convert keypoints and matches to arrays
    kp1_arr = keypoints_to_array(kp1)
    kp2_arr = keypoints_to_array(kp2)
    matches_arr = matches_to_array(good_matches)

    # Prepare inlier mask
    if matches_mask is not None:
        inlier_mask = np.array(matches_mask, dtype=np.uint8)
    else :
        inlier_mask = np.ones(len(good_matches), dtype=np.uint8)

    # Save matches and keypoints to .npz file
    np.savez(
        os.path.join(args.out_dir, f"{args.prefix}.npz"),
        kp1=kp1_arr,
        kp2=kp2_arr,
        des1=des1,
        des2=des2,
        matches=matches_arr,
        inliers=inlier_mask
    )


    # Draw matches if requested
    if args.save_image:

        # Limit number of matches to draw if specified
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

        # Save the visualization image
        img_path = os.path.join(args.out_dir, f"{args.prefix}.png")
        write_image(matched_img, img_path)

    # Print summary
    print(f"Image 1: {args.image1_path}, Keypoints: {len(kp1)}")
    print(f"Image 2: {args.image2_path}, Keypoints: {len(kp2)}")
    print(f"Total Matches Found: {len(good_matches)}")
    if matches_mask is not None:
        inliers_count = sum(matches_mask)
        print(f"Inlier Matches after RANSAC: {inliers_count}")
    else:
        print("Not enough matches for RANSAC filtering.")



if __name__ == "__main__":
    main()