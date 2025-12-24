# VisionOS Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = visionos
SRC_DIR = src
BUILD_DIR = .

# Source files
SOURCES = $(SRC_DIR)/kernel.c
OBJECTS = $(SOURCES:.c=.o)

# Default target
all: $(TARGET)

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
	@echo "  make clean    - Remove build artifacts"
	@echo "  make run      - Build and run the shell"
	@echo "  make help     - Show this help message"

.PHONY: all clean run help
