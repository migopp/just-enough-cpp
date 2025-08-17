CC 			= g++
CXX_FLAGS 	= -g -fno-omit-frame-pointer -fsanitize=address,undefined -std=c++03 -O3

SRC_DIR 	= src
BUILD_DIR	= build
EXE			= $(BUILD_DIR)/main

CXX_FILES 	= $(SRC_DIR)/main.cpp $(SRC_DIR)/Foo.cpp $(SRC_DIR)/Bar.cpp $(SRC_DIR)/Baz.cpp

all: $(EXE)

$(EXE): $(CXX_FILES)
	$(CC) $(CXX_FLAGS) src/main.cpp -o $(BUILD_DIR)/main

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
.DEFAULT: all
