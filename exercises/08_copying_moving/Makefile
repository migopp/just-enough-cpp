biv: build/biv
	@./build/biv

build/biv:  BadIntVec.cpp BadIntVec.h build
	@g++ -std=c++23 $< -o $@

build:
	@mkdir build

clean: build
	rm -rf build

.PHONY: biv clean
