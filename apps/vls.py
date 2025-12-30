import sys
import os
import logging

# Configure logging to avoid cluttering stdout which might be used for piping (though vls is usually a source)
# We will print matches to stdout.
logging.getLogger("ultralytics").setLevel(logging.ERROR)

def main():
    # Usage: python3 apps/vls.py <directory> [--contains] <label1> [label2 ...]
    if len(sys.argv) < 2:
        print("Usage: vls <directory> [--contains] <label1> [label2 ...]")
        sys.exit(1)

    target_dir = sys.argv[1]
    
    # Parse labels, ignoring "--contains"
    target_labels = set()
    for arg in sys.argv[2:]:
        if arg != "--contains":
            target_labels.add(arg.lower())

    try:
        import torch
        from ultralytics import YOLO
    except ImportError:
        print("Error: Missing dependencies. Please install ultralytics and torch.")
        sys.exit(1)

    # Check for GPU
    device = 'cuda' if torch.cuda.is_available() else 'cpu'
    
    # Load Model
    # Using yolo11n.pt (Nano, for speed)
    try:
        model = YOLO('yolo11n.pt')
    except Exception as e:
        print(f"Error loading model: {e}")
        sys.exit(1)

    # Gather Images
    valid_extensions = ('.jpg', '.jpeg', '.png', '.bmp', '.webp')
    try:
        image_files = [
            os.path.join(target_dir, f) for f in os.listdir(target_dir)
            if f.lower().endswith(valid_extensions)
        ]
    except FileNotFoundError:
        print(f"Error: Directory '{target_dir}' not found.")
        sys.exit(1)

    if not image_files:
        # No images to process
        sys.exit(0)

    # Run Inference
    # stream=True is memory efficient
    # verbose=False to keep stdout clean
    results = model(image_files, device=device, stream=True, verbose=False)

    # Filter and Output
    for result in results:
        detected_classes = set()
        if result.boxes:
            for cls_id in result.boxes.cls:
                class_name = result.names[int(cls_id)]
                detected_classes.add(class_name.lower())
        
        # Check intersection
        # If no labels provided, list all images (standard ls behavior for visual files)
        if not target_labels or not target_labels.isdisjoint(detected_classes):
            # Print filename
            print(f"{os.path.basename(result.path)}")

if __name__ == "__main__":
    main()
