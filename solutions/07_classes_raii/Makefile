foo: build/foo
	@./build/foo && \
		echo "\x1B[32mPass\x1B[0m" || \
		echo "\x1B[31mFail\x1B[0m"

build/foo:  Foo.cpp Foo.h build
	@g++ -std=c++23 $< -o $@

bar: build/bar
	@./build/bar && \
		echo "\x1B[32mPass\x1B[0m" || \
		echo "\x1B[31mFail\x1B[0m"

build/bar: Bar.cpp Bar.h build
	@g++ -std=c++23 $< -o $@

animal: build/animal
	@./build/animal && \
		echo "\x1B[32mPass\x1B[0m" || \
		echo "\x1B[31mFail\x1B[0m"

build/animal: Animal.cpp Animal.h build
	@g++ -std=c++23 $< -o $@

build:
	@mkdir build

clean: build
	rm -rf build

.PHONY: foo bar animal
