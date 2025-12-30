# VisionOS Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude -D_DEFAULT_SOURCE
TARGET = visionos
SRC_DIR = src
BUILD_DIR = .
VENV = .venv
PIP = $(VENV)/bin/pip

# Source files
SOURCES = $(SRC_DIR)/kernel.c $(SRC_DIR)/utils.c $(SRC_DIR)/executor.c
OBJECTS = $(SOURCES:.c=.o)

# Default target - does everything needed to make project work
all: check-venv make-scripts-executable $(TARGET)
	@echo ""
	@echo "✓ VisionOS is ready to use!"
	@echo "  Run with: ./$(TARGET)"

# Check if virtual environment exists, create if not
check-venv:
	@if [ ! -d "$(VENV)" ]; then \
		echo "→ Setting up Python virtual environment..."; \
		$(MAKE) setup; \
	else \
		echo "✓ Virtual environment found"; \
	fi

# Make all Python scripts executable
make-scripts-executable:
	@echo "→ Making CV scripts executable..."
	@chmod +x apps/*.py
	@echo "✓ Scripts are executable"

# Setup development environment
setup:
	@echo "→ Creating virtual environment..."
	python3 -m venv $(VENV)
	@echo "→ Installing Python dependencies..."
	$(PIP) install -r requirements.txt
	@echo "✓ Setup complete!"

# Build the executable
$(TARGET): $(SOURCES)
	@echo "→ Compiling VisionOS shell..."
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)
	@echo "✓ Build complete!"

# Clean build artifacts
clean:
	rm -f $(TARGET) $(SRC_DIR)/*.o
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

.PHONY: all setup check-venv make-scripts-executable clean run help
