import sys
import cv2
import numpy as np
import os

def read_image(source=None):
    """
    Reads an image from a file path or stdin.
    If source is None, reads from stdin buffer.
    Returns: numpy array (image) or None if failed.
    """
    if source and os.path.exists(source):
        # Read from file
        return cv2.imread(source)
    else:
        # Read from stdin
        try:
            # Read binary data from stdin
            file_bytes = np.frombuffer(sys.stdin.buffer.read(), np.uint8)
            if file_bytes.size == 0:
                return None
            return cv2.imdecode(file_bytes, cv2.IMREAD_UNCHANGED)
        except Exception as e:
            sys.stderr.write(f"Error reading from stdin: {e}\n")
            return None

def write_image(image, dest=None):
    """
    Writes an image to a file path or stdout.
    If dest is None, writes to stdout buffer.
    """
    if image is None:
        return

    if dest:
        # Write to file
        cv2.imwrite(dest, image)
    else:
        # Write to stdout
        success, encoded_image = cv2.imencode('.png', image)
        if success:
            sys.stdout.buffer.write(encoded_image.tobytes())
