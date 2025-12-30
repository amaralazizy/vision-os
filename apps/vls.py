import sys
import os
import logging
import argparse
import signal

def signal_handler(sig, frame):
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

# Configure logging to avoid cluttering stdout
logging.getLogger("ultralytics").setLevel(logging.ERROR)

VALID_EXTENSIONS = ('.jpg', '.jpeg', '.png', '.bmp', '.webp')

def parse_arguments():
    parser = argparse.ArgumentParser(description='Visual LS')
    parser.add_argument('directory', nargs='?', default='.', help='Target directory')
    parser.add_argument('-a', '--all', action='store_true', help='List all images')
    parser.add_argument('-R', '--recursive', action='store_true', help='Recursive search')
    parser.add_argument('--contains', nargs='+', help='Filter images containing these labels')
    parser.add_argument('--not-contains', nargs='+', help='Filter images NOT containing these labels')
    
    args = parser.parse_args()

    # Validation Logic
    if args.directory != '.' and not (args.all or args.contains or args.not_contains):
        print(f"Error: When specifying a directory '{args.directory}', you must specify a filter (--contains, --not-contains) or --all.")
        sys.exit(1)

    # Default behavior for current directory with no args
    if args.directory == '.' and not (args.all or args.contains or args.not_contains):
        args.all = True
        
    return args

def load_model():
    try:
        import torch
        from ultralytics import YOLO
        
        device = 'cuda' if torch.cuda.is_available() else 'cpu'
        # Using yolo11s.pt (Small, for better accuracy while still fast)
        model = YOLO('yolo11s.pt')
        return model, device
    except ImportError:
        print("Error: Missing dependencies. Please install ultralytics and torch.")
        sys.exit(1)
    except Exception as e:
        print(f"Error loading model: {e}")
        sys.exit(1)

def gather_images(target_dir, recursive=False):
    image_files = []
    
    if not os.path.exists(target_dir):
        print(f"Error: Directory '{target_dir}' not found.")
        sys.exit(1)

    if recursive:
        for root, _, files in os.walk(target_dir):
            for f in files:
                if f.lower().endswith(VALID_EXTENSIONS):
                    image_files.append(os.path.join(root, f))
    else:
        try:
            image_files = [
                os.path.join(target_dir, f) for f in os.listdir(target_dir)
                if f.lower().endswith(VALID_EXTENSIONS)
            ]
        except NotADirectoryError:
             print(f"Error: '{target_dir}' is not a directory.")
             sys.exit(1)
             
    return image_files

def filter_results(results, target_contains, target_not_contains):
    for result in results:
        detected_classes = set()
        if result.boxes:
            for cls_id in result.boxes.cls:
                class_name = result.names[int(cls_id)]
                detected_classes.add(class_name.lower())
        
        keep = True
        
        if target_contains:
            # Must contain at least one of the labels
            if target_contains.isdisjoint(detected_classes):
                keep = False
        
        if target_not_contains:
            # Must NOT contain any of the labels
            if not target_not_contains.isdisjoint(detected_classes):
                keep = False
                
        if keep:
            # Print path
            print(result.path)

def main():
    args = parse_arguments()
    
    target_contains = set(x.lower() for x in args.contains) if args.contains else None
    target_not_contains = set(x.lower() for x in args.not_contains) if args.not_contains else None

    image_files = gather_images(args.directory, args.recursive)

    if not image_files:
        sys.exit(0)

    # If --all is specified, we don't need to run inference
    if args.all:
        for img in image_files:
            print(img)
        sys.exit(0)

    model, device = load_model()

    # Run Inference
    # Lower confidence to improve recall (fix accuracy issue)
    # conf=0.05 
    results = model(image_files, device=device, stream=True, verbose=False, conf=0.05)

    filter_results(results, target_contains, target_not_contains)

if __name__ == "__main__":
    main()
