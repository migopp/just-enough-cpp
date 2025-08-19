CC 			= g++
CXX_FLAGS 	= -g -fno-omit-frame-pointer -fsanitize=address,undefined -std=c++23 -O3

SRC_DIR 	= src
BUILD_DIR	= build
EXE			= $(BUILD_DIR)/main

CXX_FILES 	= $(wildcard $(SRC_DIR)/*.cpp)

all: $(EXE)

$(EXE): $(CXX_FILES) $(BUILD_DIR)
	$(CC) $(CXX_FLAGS) $(CXX_FILES) -o $(BUILD_DIR)/main

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
.DEFAULT: all
