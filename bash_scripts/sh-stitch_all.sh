#!/usr/bin/env bash

# sh-stitch-all: Iteratively stitch all images in a folder
# Usage: ./sh-stitch-all path/to/images_dir final_output.png

IMG_DIR=$1
FINAL_OUT=$2
TEMP_OUT="temp_panorama.png"
APP_PATH="./apps/cv_stitch.py" # Adjust based on your directory structure

# Check arguments
if [[ -z "$IMG_DIR" || -z "$FINAL_OUT" ]]; then
    echo "Usage: $0 <directory_of_images> <final_output_name>"
    exit 1
fi

# Get all images in a sorted list
images=($(ls "$IMG_DIR"/*.{jpg,png,jpeg} 2>/dev/null | sort))

# Check if we have at least 2 images
if [ ${#images[@]} -lt 2 ]; then
    echo "Error: Need at least 2 images to stitch."
    exit 1
fi

echo "tarting VisionOS Bulk Stitching..."
echo "Found ${#images[@]} images."

# Step 1: Initialize by copying the first image to the temp panorama
cp "${images[0]}" "$TEMP_OUT"

# Step 2: Iterate through the rest of the images
for (( i=1; i<${#images[@]}; i++ )); do
    NEXT_IMG="${images[$i]}"

    # We use the temp panorama as image1 and the next source as image2
    python "$APP_PATH" "$TEMP_OUT" "$NEXT_IMG" --out_dir "." --out_image "current_step.png"
    
    # Check if stitching succeeded
    if [ $? -eq 0 ]; then
        mv "current_step.png" "$TEMP_OUT"
        echo "Step $i complete."
    else
        echo "Error: Stitching failed at step $i. Ensure images have overlapping features."
        exit 1
    fi
done

# Step 3: Finalize
mv "$TEMP_OUT" "$FINAL_OUT"
echo "================================================"
echo "Final panorama saved as: $FINAL_OUT"