# The Build System

**Contents**
1. [`make` Is An Executable](#make-is-an-executable)
   - [`make all`](#make-all)
   - [`make clean`](#make-clean)
   - [`.PHONY`](#phony)
2. [Variables](#variables)
   - [Special Variables](#special-variables)
3. [Assignment: `=`, `:=`, `?=`, `+=`](#assignment----)
4. [Built-In Functions](#built-in-functions)
5. [Pattern Matching and Substitution](#pattern-matching-and-substitution)
6. [Static Pattern Rules](#static-pattern-rules)
7. [Command Modifiers](#command-modifiers)
8. [Quick Reference](#quick-reference)
9. [Exercise](#exercise)

There are tons of build systems for C/C++ projects; for this class, we use the fairly low-level `Makefile`.
The syntax is not the most intuitive, so this guide is made to give you are more ground-level understanding of how it works, so that you can read Dr. Gheith's `Makefile` with less doubt. It's not a magic formula.

That being said, `make` has lots of features that I cannot possibly hope to cover here. This guide will get you started, and certainly through this class, but if you want to learn more, [refer to the manual](https://www.gnu.org/software/make/manual/html_node/).

> [!WARNING]
> If you remember the basics of `make`, you can confidently skip this section. You can always come back later if you decide you need a refresher.

## `make` Is An Executable

GNU Make is an open source language-agnostic build system used widely on Unix platforms (sorry, Windows). `make` is the binary on your machine, and you can invoke it as such.
You can also see which `make` binary you are using and which version it is:

``` sh
# Prints the path to the binary run when you call `make`.
which make

# Version of `make` on your system.
make --version
```

For example, my results are:

``` sh
> which make
/usr/bin/make

> make --version
GNU Make 3.81
Copyright (C) 2006  Free Software Foundation, Inc.
This is free software; see the source for copying conditions.
There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.

This program built for i386-apple-darwin11.3.0
```

If you are comfortable with the fact that `make` is an executable like any other, let's go ahead and cover how we can use it.
Let's first just play around and see what happens when we call make in an empty directory.

``` sh
mkdir -p test && cd test; make; echo $?
```

You should get something like:

``` sh
make: *** No targets specified and no makefile found.  Stop.
2
```

So, when you run `make`, it looks in your current working directory, and looks for a `Makefile`[^1]. Here, it expects to find _targets_.

## Targets

A _target_ is just a name of a task for make to complete. It can be directly tied to a file (i.e., a target can exist to create a certain file), or it can just be a descriptive name that you choose (i.e., a `run` target may run an executable, without needing to actually build anything). The latter group are referred to as phony targets.

Specifying a basic target within a `Makefile` uses the following syntax:

``` makefile
TargetName: Dependencies 
    CommandsToRun
```

For example, let's make that `run` target described above, which simply runs a binary (let's say, `./build/main`).

``` makefile
run: build/main
    ./build/main
```

What about a target to _create_ the `build/main` binary? Let's suppose we have a `src/main.cpp` file. Then, it might look like this:

``` makefile
build/main: build build/main.o
    g++ build/main.o -o build/main

build/main.o: build src/main.cpp
    g++ -c src/main.cpp -o build/main.o

build:
    mkdir -p build
```

We've split the compilation process into two steps, one that compiles the `build/main.o` _object file_ (compiles without linking), and another one that links the object file into an _executable_. We could easily have just made it compile it all within one target like so:

``` makefile
build/main: src/main.cpp
	mkdir -p build
    g++ src/main.cpp -o build/main
```

So, how do we specify the target when we run `make`? We simply type the name of the target we would like to run:

``` sh
make build/main
```

If you run just `make` without specifying a target, it will only run the first target in your `Makefile`. You can change this with the `.DEFAULT` variable.

### `make all`

The `all` target is the somewhat idiomatic entry point for a `Makefile`. It's just convention, but the first target tends to be `all`, and that tends to build the project in the way you'd expect (i.e., it would be weird to have an `all` target that prints "Hello, world!" and then dies).

### `make clean`

The `clean` target is another idiomatic target found in many `Makefile` setup. It's purpose is to clean up any build artifacts. There is also its older brother, `spotless`, which _really_ cleans things up, making everything like new. Many times, creating a `clean` target that just removes the build artifact folder is enough.

### `.PHONY`

You may also see something like the following in a `Makefile`:

``` makefile
.PHONY: all test clean
```

This `.PHONY` variable informs `make` that a group of targets are phony (meaning they are not backed by any real file). In this case, it labels `all`, `test`, and `clean` as phony targets.

## Variables

`make` also has a notion of variables, and you'll likely see them quite a bit. The syntax to declare them looks like this:

``` makefile
VARIABLE_NAME = value
```

You can make multiline variables like this:

``` makefile
VARIABLE_NAME = value_one \
    value_two \
    value_three
```

It's convention that variables are written in the screaming text, like shell variables.
Though, it is important to know that `make` variables are _not_ shell variables. That is, shell variables are not available as `make` variables specially creating them.

You can access a `make` variable via `$(VARIABLE_NAME)` (most idiomatic) or `${VARIABLE_NAME}`.

To assign a shell variable to a `make` variable, you will need to use the `shell` built-in function, and escape the name of the variable with an extra `$` token:

``` makefile
MY_SHELL = $(shell echo $$SHELL)
```

### Special Variables

`make` has a number of special variables available for use within target rules.
A few important ones include:

- `$@`: The file name of the target.
  ``` makefile 
  hello:
      echo $@ # hello
  ```
- `$<`: The first prerequisite.
  ``` makefile
  hello: prereq
      echo $< # prereq
  ```
- `$^`: All prerequisites.
  ``` makefile
  hello: prereq1 prereq2
      echo $^ # prereq1 prereq2
  ```

A full list is available [here](https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html).

## Assignment: `=`, `:=`, `?=`, `+=`

The `=` is the default operator to assign variables, but it does recursive expansion.
This means that it's lazily evaluated. For example:

``` makefile
DATE = $(shell date)
```

Whenever `$(DATE)` is used, the command on the right-hand side is run and substituted in.

The `:=` operator runs the command immediately, so:

``` makefile
DATE := $(shell date)
```

Will run `date` when the variable `DATE` is created and subtitute that value every time `$(DATE)` is used.

The `?=` is used for conditional assignment; that is, if the value was not defined before.

``` makefile
SHELL ?= "/bin/zsh"
```

Then, when you call `make`, you can override them:

``` sh
make SHELL="/bin/bash"
```

Not that if you use `?=`, the left-hand side is lazily evaluated.

The `+=` works exactly as you expect it would. It appends text to the end of a variable.

``` makefile
CFLAGS += -g
```

## Built-In Functions

You've seen the `$(shell ...)` built-in function, but there are a whole list of others [here](https://www.gnu.org/software/make/manual/html_node/Functions.html) if you are interested in learning them.

Some good ones to explore are:
- `let`
- `foreach`
- `info`

## Pattern Matching and Substitution

But, what if we have a ton of `.cpp` source files? Nobody wants to go modify their `Makefile` every time they add or rename a file in their project. To this end, there are some nice pattern matching and substitution built-in functions in `make`.

One of these is `wildcard`[^2]:

``` makefile
SRC_DIR = src
CPP_FILES = $(wildcard $(SRC_DIR)/*.cpp)
```

Then, `CPP_FILES` will be a list of all the files ending in `.cpp` in `src/`. The `*` token says to match any text.

We can also use `patsubst`[^3]:

``` makefile
BUILD_DIR = build
O_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CPP_FILES))
```

This says to take the list in `CPP_FILES`, and change each entry from `src/<filename>.cpp` to `build/<filename>.o`, and store the result list in the variable `O_FILES`.

The `%` token matches anything similarly to `*`, but can only appear once per pattern, and the text matched is replaced exactly as-is in the result (i.e., `<filename>` will be exactly the same in the pattern text and output text in the above example).

Build rules also have support for pattern matching.

``` makefile
%.o: %.cpp
    g++ $< -o $@
```

This is a rule for creating a object file from any `.cpp` file passed in.

## Static Pattern Rules

You may also see these sometimes.

``` makefile
OBJ_FILES = build/obj/server.o build/obj/client.o

$(OBJ_FILES): build/obj/%.o: src/%.cpp
    g++ $< -o $@
```

They are called static pattern rules, and they are simply a way to apply a generic rule over a known collection of files[^4].

## Command Modifiers

Another thing you are likely to see is something of the form:

``` makefile
target: prereq
    @echo "Hello, world!"
```

By default, `make` prints every command that it runs. This `@` token tells `make` that it shouldn't print this one; it should be silent[^5].

Otherwise, we would see output of the form:

``` sh
echo "Hello, world!"
Hello, world!
```

You may also see something like:

``` makefile
target: prereq
    -g++ $< -o $@
```

This `-` token tells `make` to _ignore errors_ in the line which proceeds[^6].

Other command modifiers are specified in [§5](https://www.gnu.org/software/make/manual/html_node/Recipes.html) of the GNU Make manual.

## Quick Reference

If you ever want a quick reference to the commands available in GNU Make, [here](https://www.gnu.org/software/make/manual/html_node/Quick-Reference.html) is a good link that I use.


## Exercise

You have read a lot about `make`. Let's be practical and try to put your skills to the test.

In this directory, a sample C++ project is set up, but it fails to compile because I failed to write the `Makefile` correctly. Oops.

- I may have used the wrong C++ version... I'd like to use features from the C++23 standard.
- Not all of my `.cpp` files are being compiled.
- My target to create my `build` directory is not working.

Can you fix the issues for me? You should not have to touch anything other than the `Makefile`.

[^1]: Or, the equivalent file under a few other valid names: https://www.gnu.org/software/make/manual/html_node/Makefile-Names.html

[^2]: https://www.gnu.org/software/make/manual/html_node/File-Name-Functions.html#index-wildcard-2

[^3]: https://www.gnu.org/software/make/manual/html_node/Text-Functions.html#index-patsubst-1

[^4]: https://www.gnu.org/software/make/manual/html_node/Static-Pattern.html

[^5]: https://www.gnu.org/software/make/manual/html_node/Echoing.html

[^6]: https://www.gnu.org/software/make/manual/html_node/Errors.html
