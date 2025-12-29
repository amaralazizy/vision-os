import sys
import argparse
from cv_utils import read_image



def print_basic_info(img, path):
    h, w = img.shape[:2]
    channels = 1 if img.ndim == 2 else img.shape[2]

    print("=== Basic Image Info ===")
    print(f"Path          : {path}")
    print(f"Width         : {w}")
    print(f"Height        : {h}")
    print(f"Channels      : {channels}")
    print(f"Data type     : {img.dtype}")
    print(f"Total pixels  : {w * h}")
    print()


def print_statistics(img):
    print("=== Image Statistics ===")

    if img.ndim == 2:  # Grayscale
        print(f"Min value     : {img.min()}")
        print(f"Max value     : {img.max()}")
        print(f"Mean          : {img.mean():.3f}")
        print(f"Std deviation : {img.std():.3f}")
    else:
        for i, name in enumerate(["B", "G", "R"]):
            channel = img[:, :, i]
            print(f"{name} channel:")
            print(f"  Min         : {channel.min()}")
            print(f"  Max         : {channel.max()}")
            print(f"  Mean        : {channel.mean():.3f}")
            print(f"  Std         : {channel.std():.3f}")

    print()


def main():
    parser = argparse.ArgumentParser(description="Display image information and metadata")
    parser.add_argument("image_path", help="Path to input image")

    args = parser.parse_args()

    img = read_image(args.image_path)
    if img is None:
        sys.stderr.write("Error: Could not read image\n")
        sys.exit(1)

    print_basic_info(img, args.image_path)
    print_statistics(img)


if __name__ == "__main__":
    main()
