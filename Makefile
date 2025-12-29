# VisionOS Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = visionos
SRC_DIR = src
BUILD_DIR = .
VENV = .venv
PIP = $(VENV)/bin/pip

# Source files
SOURCES = $(SRC_DIR)/kernel.c
OBJECTS = $(SOURCES:.c=.o)

# Default target
all: $(TARGET)

# Setup development environment
setup:
	python3 -m venv $(VENV)
	$(PIP) install -r requirements.txt
	@echo "Setup complete! Virtual environment created in $(VENV)"

# Build the executable
$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)
	@echo "Build complete! Run with: ./$(TARGET)"

# Clean build artifacts
clean:
	rm -f $(TARGET) $(OBJECTS)
	@echo "Clean complete!"

# Run the shell
run: $(TARGET)
	./$(TARGET)

# Help target
help:
	@echo "VisionOS Makefile"
	@echo "================="
	@echo "Targets:"
	@echo "  make          - Build the VisionOS shell"
	@echo "  make setup    - Create virtual environment and install dependencies"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make run      - Build and run the shell"
	@echo "  make help     - Show this help message"

.PHONY: all setup clean run help
