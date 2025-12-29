# VisionOS

A C-based shell with integrated computer vision capabilities using OpenCV.

## Overview

VisionOS is a custom shell implementation that provides:
- Standard command execution (e.g., `ls`, `pwd`, `echo`)
- Special `cv-` prefixed commands that execute Python scripts with OpenCV for computer vision tasks
- Process management using `fork()`, `exec()`, and `wait()`

## Features

- **CLI Prompt**: Interactive command-line interface with infinite loop
- **Command Parsing**: Intelligent parsing of user input
- **Dual Execution Modes**:
  - Standard commands: Execute using `execvp()` (e.g., `ls`, `pwd`)
  - CV commands: Execute Python scripts with OpenCV (e.g., `cv-show img.jpg`)
- **Process Management**: Proper process forking and waiting

## Project Structure

```
vision-os/
├── src/
│   └── kernel.c          # Main shell implementation
├── apps/
│   └── cv_show.py        # OpenCV image display script
├── Makefile              # Build configuration
├── .gitignore            # Git ignore rules
└── README.md             # This file
```

## Prerequisites

### System Requirements
- GCC compiler
- Python 3
- OpenCV for Python

### Installing Dependencies

1. Install system requirements (GCC and Python 3):
   ```bash
   # On Ubuntu/Debian:
   sudo apt-get install build-essential python3 python3-pip python3-venv
   ```

2. Run the project setup:
   ```bash
   make setup
   ```
   This will create a virtual environment in `.venv` and install the required Python packages (`opencv-python`, `numpy`).

## Building

Build the VisionOS shell using make:

```bash
make
```

This will compile `kernel.c` and create the `visionos` executable.

## Running

Start the VisionOS shell:

```bash
./visionos
```

Or use make:

```bash
make run
```

## Usage

### Standard Commands

Execute any standard Unix command:

```bash
visionos> ls
visionos> pwd
visionos> echo "Hello, VisionOS!"
visionos> date
```

### Computer Vision Commands

Commands starting with `cv-` are executed as Python scripts with OpenCV:

```bash
# Display an image
visionos> cv-show image.jpg
```

The shell will automatically:
1. Detect the `cv-` prefix
2. Fork a new process
3. Execute `apps/cv_show.py` with the provided arguments
4. Wait for the process to complete

### Exit

To exit the shell:

```bash
visionos> exit
```

Or press `Ctrl+D`.

## Implementation Details

### kernel.c

The main shell implementation includes:

1. **Infinite Loop**: Continuously prompts for user input
2. **Input Parsing**: Tokenizes input into command and arguments
3. **Command Detection**: Checks for `cv-` prefix
4. **Process Forking**: Creates child processes for command execution
5. **Execution**:
   - CV commands: `fork()` + `exec()` Python script
   - Standard commands: `fork()` + `execvp()`
6. **Process Management**: `wait()` for child processes to complete

### cv_show.py

A Python script that:
- Takes an image path as argument
- Loads the image using OpenCV
- Displays image information (dimensions, channels)
- Shows the image in a window
- Waits for user keypress to close

## Example Session

```bash
$ ./visionos
VisionOS Shell v1.0
Type 'exit' to quit
====================================

visionos> ls
Makefile  README.md  apps  src  visionos

visionos> pwd
/home/user/vision-os

visionos> cv-show test_image.jpg
Image: test_image.jpg
Dimensions: 800x600
Channels: 3

Displaying image... Press any key to close.

visionos> exit
Exiting VisionOS Shell...
```

## Development

### Clean Build Artifacts

```bash
make clean
```

### Adding New CV Commands

To add a new computer vision command:

1. Create a new Python script in `apps/` directory (e.g., `cv_filter.py`)
2. Make it executable: `chmod +x apps/cv_filter.py`
3. Use the command: `cv-filter <args>`

The shell will automatically find and execute `apps/cv_filter.py`.

## Testing

### Test Standard Commands

```bash
./visionos
visionos> ls
visionos> echo "test"
visionos> pwd
```

### Test CV Commands

First, create or download a test image, then:

```bash
./visionos
visionos> cv-show test.jpg
```

## Troubleshooting

### OpenCV Not Found

If you get "OpenCV (cv2) is not installed" error:

```bash
pip3 install opencv-python
```

### Command Not Found

If standard commands fail:
- Ensure the command exists in your PATH
- Check command spelling

### Permission Denied

If you get permission errors:
- Ensure Python scripts are executable: `chmod +x apps/*.py`
- Check file permissions on image files

## License

This project is for educational purposes.

## Contributing

Contributions are welcome! Please feel free to submit issues or pull requests.