# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Iinclude -std=c++11

# SDL2, SDL2_image, and SDL2_ttf library flags
SDL2_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf

# Directories
SRC_DIR = src
INCLUDE_DIR = include
IMAGE_DIR = images
BUILD_DIR = build

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)

# Output executable
OUTPUT = chess

# Default target
all: $(BUILD_DIR) $(OUTPUT)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile the program
$(OUTPUT): $(SRC_FILES)
	$(CXX) $(CXXFLAGS) $(SRC_FILES) -o $(BUILD_DIR)/$(OUTPUT) $(SDL2_FLAGS)

# Clean up build directory and executable
clean:
	rm -rf $(BUILD_DIR)

# Run the program
run: all
	./$(BUILD_DIR)/$(OUTPUT)

.PHONY: all clean run
