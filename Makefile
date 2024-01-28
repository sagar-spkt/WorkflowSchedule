# Compiler and Compiler Flags
CXX = g++
CXXFLAGS = -std=c++11 -I workflow

# Source and Build Directories
SRC_DIR = .
BUILD_DIR = build

# Source Files and Object Files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# Target executable
TARGET = main

# Rule to build the target executable
$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to build object files from source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Rule to create the build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $@

# Clean target to remove build artifacts
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

# Run target to build and execute the program
.PHONY: run
run: $(BUILD_DIR)/$(TARGET)
	./$(BUILD_DIR)/$(TARGET)
