# `#include` 

**Contents**
1. [The Preprocessor](#the-preprocessor)
   - [Define](#eg-define)
2. [The Directive](#the-directive)
3. [Pitfalls](#pitfalls)
   - [Multiple Inclusion](#multiple-inclusion)
     - [`#ifndef` Guards](#ifndef-guards)
     - [`#pragma once`](#pragma-once)
     - [Always Use Include Protection](#always-use-include-protection)
   - [Multiple Definitions](#multiple-definitions)
     - [Header Versus Source Files](#header-versus-source-files)
   - [Circular Include](#circular-include)
     - [Forward Declaration](#forward-declaration)
     - [General Advice](#general-advice)
 4. [`<>` Versus `""`](#angle-brackets-versus-quotes)
     

Let's start out nice and simple with the `#include` preprocessor directive.
This is the mechanism used to share interfaces and data over multiple C++ source/header files.

> [!WARNING]
> If you are confident with the mechanics and semantics of the `#include` directive, you can skip this section, though I would recommend taking a cursory glance through the content and convincing yourself that you it all.
>
> Issues with `#include` are quite common in this class. Things get complicated with many source/header files. Particularly the issue of circular dependencies and multiple inclusion. This section will give you a grounding point of understanding, as well as mechanisms to use in debugging these issues.

## The Preprocessor
I'm sure you are already aware of the high-level details of the C preprocessor.
The principles are nearly identical over in C++ land.

Just in case you forgot, though: the C preprocessor (also `cpp` for short, sorry) is a program that takes a C or C++ source file and modifies it ahead of compilation time according to directives specified by the programmer.

It doesn't require any work on your part to take advantage of it, though!
Modern compiler toolchains like `gcc` (formerly known was the GNU C Compiler, now known as the GNU Compiler Collection[^1]) have it built-in.
That is to say, when you type `gcc main.c -o main.o`, the pipeline really looks like this:

~~~
Source Code -> C Preprocessor -> Compiler -> Assembler -> Linker
~~~

I know it's named the C preprocessor, the same principle applies to C++ with some minor differences that you don't need to care about for now[^2].

### e.g., `define`

You may recall the `#define` directive that allows you to create a compile-time-constant symbol.

``` c++
#define PI 3

int main()
{
    return PI;
}
```

In the above program, `PI` is _not_ a symbol that the compiler is aware of.
After the C Preprocessor runs, the source code looks (mostly) like this:

``` c++
int main()
{
    return 3;
}
```

Then, it's clear that `#define` simply textually substitutes the left part of the definition with the right part of the definition in the source file.

But please, don't take my word for it. You are welcome to see this fact for yourself. In the `define` directory, you will find the `Pi.cpp` source file. Go ahead and `cd define`. Open a shell, and feed in the following command (The `-E` flag tells the toolchain to _stop_ after the preprocessing step):

``` sh
g++ -E Pi.cpp
```

You should see the following:

``` c++
# 1 "Pi.cpp"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 513 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "Pi.cpp" 2


int main()
{
    return 3;
}
```

The lines at the top are line markers, used internally by the compiler.
In fact, any source code that is fed into the preprocessor comes out with line markers.
You can tell the preprocessor to omit them with the `-P` flag.

Thus, running:

``` sh
g++ -E Pi.cpp -P
```

Results in:

``` c++
int main()
{
    return 3;
}
```

## The Directive

Now, you probably have a pretty good guess what this `#include` does.

> It textually substitutes the contents of the file specified to the right of the `#include` directive?

Wow. How did you speak like that? That's weird. But yes weird head voice, that's basically what it does.

We can observe this first-hand by turning our attention to the `include` directory. 
Go ahead and `cd include` to make it your current working directory.
Within, there are the `Even.h` and `main.cpp` files.


``` c++
// Even.h
namespace jecpp {

/// The best function for determining if an input is even.
///
/// They've never seen something like it... And they didn't even believe it was possible.
/// "Oh, you have the best `isEven` function," they said.
bool isEven(int n)
{
    return (n & 1) == 0;
}

} // namespace jecpp
```

You may notice a linter warning on the `jecpp::isEven` function...
Ignore it for now, we will talk about it in a second.

For now, notice that we have a simple function in a namespace.
We'd like to be able to use this functionality in our main program.

Taking a look:

``` c++
// main.cpp
#include "Even.h"

int main()
{
    return jecpp::isEven(2) ? 0 : 1;
    // In case you have never seen the ternary operator, this is equivalent:
    //
    // ```cpp
    // if(jecpp::isEven(2)) {
    //     return 0;
    // } else {
    //     return 1;
    // }
    // ```
}
```

It seems like we have an `#include` directive telling the preprocessor to open the file called `Even.h`, gather it's contents, and paste them in the body of `main.cpp`.

We can check exactly what the preprocessor is doing:

``` sh
g++ -E main.cpp -P
```

And we get:

``` c++
namespace jecpp {
bool isEven(int n)
{
    return (n & 1) == 0;
}
}

int main()
{
    return jecpp::isEven(2) ? 0 : 1;
}
```

Well, it stripped all of our comments. But this is basically what we expected right?

We can make sure it works by using the following command:

``` sh
# Compile into a binary
mkdir -p build; g++ main.cpp -o build/even; \
# Run the binary and then report the output code
./build/even; echo $?
```

You should see a 0 printed to the console.
Go ahead and play with the value passed into `jecpp::isEven()` and see the different error codes reported.

## Pitfalls

The paradigm shown above is all fine and dandy for our toy example, but in real programs, this basic approach often doesn't work.

### Multiple Inclusion

Let's look at the `multiple_include` directory.
Go ahead and `cd` in as usual.
You'll find the same `Even.h`, along with a new `Odd.h` and `SuperEven.h`, as well as a modified `main.cpp`.

Looking at `Odd.h`:

``` c++
#include "Even.h"

namespace jecpp {

bool isOdd(int n)
{
    return !isEven(n);
}

} // namespace jecpp
```

It looks like it makes use of `Even.h`. Nice code reuse!

What about `SuperEven.h`?

``` c++
#include "Even.h"

namespace jecpp {

bool isSuperEven(int n)
{
    return isEven(n) && isEven(n + 2);
}

} // namespace jecpp
```

Also straightforward. Super useful and practical, right?
And `main.cpp` just makes calls to our newly-defined helper functions.

``` c++
#include "Odd.h"
#include "SuperEven.h"

int main()
{
    return jecpp::isOdd(2) || jecpp::isSuperEven(2) ? 0 : 1;
}
```

But when we compile like before:

``` sh
mkdir -p build && g++ main.cpp -o build/magic && ./build/magic; echo $?
```

We get an error...

``` sh
In file included from main.cpp:2:
In file included from ./SuperEven.h:1:
./Even.h:7:6: error: redefinition of 'isEven'
    7 | bool isEven(int n)
      |      ^
./Even.h:7:6: note: previous definition is here
    7 | bool isEven(int n)
      |      ^
1 error generated.
zsh: no such file or directory: ./build/magic
127
```

But, we only had one `jecpp::isEven()` function, right?
We included the same file so that each `Odd.h` and `SuperEven.h` had a copy.

I bet you can guess what happened here, but let's check the preprocessor output just in case:

``` sh
g++ -E main.cpp -P
```

And we should get:

``` c++
namespace jecpp {
bool isEven(int n)
{
    return (n & 1) == 0;
}
}

namespace jecpp {
bool isOdd(int n)
{
    return !isEven(n);
}
}
namespace jecpp {
bool isEven(int n)
{
    return (n & 1) == 0;
}
}

namespace jecpp {
bool isSuperEven(int n)
{
    return isEven(n) && isEven(n + 2);
}
}

int main()
{
    return jecpp::isOdd(2) || jecpp::isSuperEven(2) ? 0 : 1;
}
```

Turns out that since `Odd.h` and `SuperEven.h` _both_ include a copy of `Even.h`, each of their copies is pasted in post-processed source of `main.cpp`.

You can think of the `#include` directive as working recursively on the pasted files.
It starts at `main.cpp`, sees that `Odd.h` is included.
Then, it must process `Odd.h` and paste it's contents.
In processing `Odd.h` it sees that `Even.h` is included, so it must process the contents of `Even.h`.
`Even.h` includes no other files, and requires no other work of the preprocessor, so the contents are pasted in `Odd.h`. Similarly, the contents of `Odd.h` are pasted into `main.cpp`, then the second include of `Super.h` is processed, and so on.

This is evaluation in what we call depth-first order.

As for solutions, there are a few!

#### `#ifndef` Guards

This is what you are most likely to see across the kernel codebase.

The `ifdef` (if defined) directive tells the preprocessor to include a certain piece of code _only_ if a certain symbol is defined by the preprocessor `define` macro. That is:

``` c++
#define FOO // Yes, you can do this. It just defines a symbol `Foo` with no value.

#ifdef FOO

// This code is always included because we just defined `Foo`.
int main()
{
    printf("Foo\n");
    return 0;
}

#endif
```

Then, `ifndef` (if _not_ defined) tells the preprocessor to include a piece of code in the case that a symbol is not defined.

``` c++
#ifndef BAR

// This is never included, because `BAR` is defined nowhere.
int main()
{
    printf("Bar\n");
    return 0;
}

#endif
```

This seems useless at first, but we can make sure that a piece of code is only ever included once by using the following structure:

``` c++
#ifndef BAZ
#define BAZ

int main()
{
    printf("Baz\n");
    return 0;
}

#endif
```

These are what we call include guards[^3].
And, they are the most legit mechanism by which to force inclusion of a file only once.

Defining random symbols is a little dangerous, though.
This is because in a large program, how do you know `BAZ` is not defined elsewhere for an entirely different purpose?
Then, the guards don't work at all!

This is why we have a special naming convention for include guards.
Each style guide will have it's own preference, but it is usually something of the form:

``` c++
// Binky.H

#ifndef BINKY_H
#define BINKY_H
// ...
#endif
```

Having your include guard be the name of the file is a good start. You likely won't have multiple header files of the same name (certainly not in this class).

But in the case that you do, some style guides suggest that the full path from the project source be included:

``` c++
// src/include/Syscalls.h

#ifndef SRC_INCLUDE_SYSCALLS_H
#define SRC_INCLUDE_SYSCALLS_H
// ...
#endif
```

Armed with this knowledge, you can now fix `Even.h`.
Try it out if you like, then you can attempt to recompile with the same command as above. It should work this time and report an exit code of 0.

#### `#pragma once`

While the include guards are guaranteed to work across every compiler, they are a bit clunky looking (and were clearly some work to explain).

There is another (more unofficial) mechanism to prevent multiple inclusion, making use of the `pragma` directive.
Which, in general, is just a way of feeding more information to the compiler[^4]. What information can be passed is implementation-dependent, but it's also not important for you to know right now.

Passing the `once` literal after the `pragma` directive is interpreted across all major compiler toolchains as an instruction to only process that file a single time[^5].

For example, in almost all cases, the following two examples are equivalent:

``` c++
// Foo.h

#pragma once

int main()
{
    // ...
}
```

``` c++
// Foo.h

#ifndef FOO_H
#define FOO_H

int main()
{
    // ...
}

#endif
```

It's simply a matter of preference what you pick.
You'll likely see both in your time with C++.
Include guards are technically safer[^6], since they are guaranteed to work according to the standard, but all standard compiler toolchains must support the `pragma once` for compatability reasons.

`Even.h` can also be fixed this way. Give it a whirl if you like.

#### Always Use Include Protection

The takeaway here is _ALWAYS, ALWAYS, ALWAYS_ use include protection of some sort. It is very important that you do so. You can usually use whichever you like, but you must use at least one.

### Multiple Definitions

Take a look at the `multiple_definitions` directory.
Make it your current working directory.

Then, take a little look at the files within.
`Even.h` makes a return, but this time we a `#pragma once` directive!

We then have two `.cpp` source files which will act as libraries to link into our `main.cpp` file (though, they are not actually used out of simplicity). Each of these source files include `Even.h`.

We can build our binary like so:

``` sh
mkdir -p build && \
# Compile _only_. Do not link.
g++ -c MyLib.cpp -o build/MyLib.o && \
g++ -c MyUtil.cpp -o build/MyUtil.o && \
# Compiles `main.cpp` and links in the other object files.
g++ main.cpp build/MyLib.o build/MyUtil.o -o build/main && \
./build/main; echo $
```

This time, it's not the compiler that fails, but the linker:

``` sh
duplicate symbol 'jecpp::isEven(int)' in:
/Users/migopp/Repo/just-enough-cpp/exercises/1_include/multiple_definitions/build/MyUtil.o
/Users/migopp/Repo/just-enough-cpp/exercises/1_include/multiple_definitions/build/MyLib.o
ld: 1 duplicate symbols
clang++: error: linker command failed with exit code 1 (use -v to see invocation)
```

So, clearly our `#pragma once` wasn't actually enough.
This is because we've compiled `MyLib.cpp` and `MyUtil.cpp` _separately_. Then in each translation unit, the compiler followed directions and included `Even.h` only once, but when we went to link the two object files together, they each had their own copy.
How does the linker know which to call when `jecpp::isEven()` is called?

_We_ know that it doesn't matter because they are the same function, but there's no reliable way for the linker to figure that out. At least, we haven't devised a way yet.

This poses an pretty major issue, because the `MyUtil` and `MyLib` libraries _need_ access to `jecpp::isEven()`.
They must both include `Even.h`. So it seems like we are doomed.

#### Header Versus Source Files

In fact, you may have heard that header files are for declarations only. You should then create a corresponding definition in a `.cpp` source file. This is the reason why.

We can create a definition for a function by omitting the body. Then, our new setup should look something like:

``` c++
// Even.h

#pragma once

namespace jecpp {

bool isEven(int n);
// The `n` is optional.
//
// So we could just write:
// bool isEven(int);
//
// In fact, that is more of the C-style.
// You may or may not see it from time to time.

} // namespace jecpp
```

``` c++
// Even.cpp

#include "Even.h"

// This is a shorthand for encapsulating `isEven()` within the `jecpp` namespace.
bool jecpp::isEven(int n)
{
    return (n & 1) == 0;
}
```

Feel free to try this out, and then run the following compile command, notice how `Even.cpp` needs to be compiled added in as a separate translation unit so that the linker can resolve the `jecpp::isEven()` symbol.

``` sh
mkdir -p build && \
# Compile _only_. Do not link.
g++ -c MyLib.cpp -o build/MyLib.o && \
g++ -c MyUtil.cpp -o build/MyUtil.o && \
# Compiles `main.cpp` and links in the other object files.
g++ main.cpp Even.cpp build/MyLib.o build/MyUtil.o -o build/main && \
./build/main; echo $
```

You should see the build complete, the binary run, and return an exit code of 0.

### Circular Include

Circular inclusion is one of these problems that tends to be quite tricky for students, and it comes up in this class because of the heavy use of templated functions (which we will discuss in more detail later). For now, just know that templated functions are generic across multiple types and so are declared _and_ defined in the header file. In such cases, the body of the function is likely to depend on the API in another header file.

Though, talk is cheap. Let's walk through an example. As per usual, take a look at `circular_include` and make it your current working directory.

We haven't talked about classes yet, so we will limit our discussion to simple _Plain Old Data_ structs (POD, meaning simply other POD fields; i.e., no constructors, destructors, or virtual member functions), which are the same structs you should be familiar with from your work in C.

Take a look at `Vertex.h` and `Edge.h`.
A `Vertex` is defined as a value along with a list of `Edge` objects.
An `Edge` is a weight along with a starting and ending `Vertex`.

We can see even from the definition that this is cyclical:

``` c++
struct Vertex {
    int value;
    // Don't know how many elements to allocated with each vertex, so we just decay it to a pointer.
    Edge* edgeList;
};
```

``` c++
struct Edge {
    int weight;
    Vertex start;
    Vertex end;
};
```

As such, `Vertex.h` includes `Edge.h`, and `Edge.h` includes `Vertex.h`.

From what we know about the preprocessor, this probably won't work. After all, suppose that `Edge.h` is processed first, then the preprocessor fetches the contents and finds the directive to include `Vertex.h`, it then goes to process the contents of `Vertex.h` and finds an instruction to include `Edge.h`. And, it's found itself back where it started.

At this point, the headers are protected by include guards, the preprocessor finds no contents in `Edge.h`, and if they are protected via `pragma once`, then the preprocessor won't search the same file again. In either case, it moves on, and one definition is placed in the translation unit before the other. Suppose `Vertex` is first. Then, when the compiler goes to process `Vertex`, it sees a reference to a type `Edge`, and does not recognize the symbol.

Let's go ahead and try to compile what we have for the fun of it. Just to make sure that I am not speaking complete nonsense.

``` sh
mkdir -p build; g++ main.cpp -o build/main; echo $?
```

And our suspicions are confirmed:

``` sh
In file included from main.cpp:1:
In file included from ./Edge.h:3:
./Vertex.h:10:5: error: unknown type name 'Edge'
   10 |     Edge* edgeList;
      |     ^
1 error generated.
1
```

But, how might we solve this issue?
`Vertex` and `Edge` seem too coupled. There is no artificial dependency here.

We could solve this issue using some fancy C++ features, but it's solvable without them. This type of dependency happens to come up quite often, and the following technique is almost universally helpful in solving it.

#### Forward Declaration

You may have seen or used this technique before you learned about headers files.

Consider the following C file:

``` c
#include <stdio.h>

void hello(const char *name);

int main(void)
{
    hello("Dr. Gheith");
    return 0;
}

void hello(const char *name)
{
    printf("Hello, %s!", name);
}
```

The `hello` function is declared above `main` so that it can be used without relegating `main` to the bottom of the file.

This is _forward-declaring_ the `hello` function. We specify the signature so that the compiler is able to verify that the call in `main` is valid.

This same technique works for structs (and classes in C++):

``` c
struct Point;

int main(void)
{
    struct Point *point_ptr = NULL;
    return 0;
}

struct Point {
    int x;
    int y; 
};
```

This does come with a few limitations, though.

1. If we forward declare a type, we can _only_ declare pointers and references to variables of that type. This is simply because the compiler doesn't know how much space to allocate for them without the full definition.
2. We can't access any members or fields of the forward-declared type without the full definition.

These seem quite debilitating, but oftentimes this is all we need. To say that something exists is oftentimes enough.

Let's go back to our example with `Vertex.h` and `Edge.h`. Notice that our `Vertex` struct stores a pointer to type `Edge` because we've represented an array as a pointer. This is a prime case for forward declaration. We can remove the `include` statement and simply forward declare the `Edge` struct.

``` c++
// Vertex.h

#pragma once

namespace jecpp {

// Forward declare.
struct Edge;

struct Vertex {
    int value;
    Edge* edgeList;
};

} // namespace jecpp
```

Then, we don't have the dependency cycle anymore! `Vertex` knows that `Edge` exists without having to include the entire header file.

Now, if you try the same compilation command as before, you should see it exit with return code 0, indicating success.

We could also have chosen to forward declare `Vertex` in `Edge.h`, and then remove the include directive in `Edge.h` (in fact, you should do this and make the appropriate modifications to the `Edge` struct to convince yourself that you understand the concepts). We also could have removed the `include` from _both_ `Edge.h` _and_ `Vertex.h`.

Now, forward declarations don't _always_ work. Particularly for templated entities (if you don't know what those are right now, it is OK), which tend one of the larger causes of circular dependencies in this class.
But, it's a simple and common technique to grab first.

That being said, I do have some general advice.

#### General Advice

If you are struggling with circular includes, it's likely because of one of these:

1. Your data is too coupled with your API.

   This might be true if you expose a piece of data in a header file rather than using the `extern` keyword to tell the compiler that the symbol will be resolved by another translation unit at link-time. If you expose a piece of data in a header with a separate API, then you may include that header only to access that data, and cause a false dependency.

2. You have too many header files with interdependencies.

   This might cause a cyclic include in the case that you have many small headers which fundamentally form a single API, so many headers include other headers.

3. Your header files are too broad and should be split up.

   This might cause cyclic includes in that many APIs are crammed into a single header. Suppose that there is a large header file with two distinct APIs. Then, there is a separate header file which depends on one of the API's in the large header. In the case that the other API in the large header depends ona the API in the smaller header, a circular include cycle is formed.
   
At this point, you may just be thinking that you can't win. Your headers can't be too large, or too small. You can't mix your data and API too much. Good design is hard. It comes with time.

To be honest, there are still some situations that I've dug myself into with code that I _don't know how to get myself out of cleanly_. Such is life sometimes, I think.

## Angle Brackets Versus Quotes

I've saved this for last because it's not really the most relevant topic for this class, so its OK if you got bored and stopped reading by now.

_But_, I have seen this topic come up in test cases later in the class, and I figure it's moderately helpful, so I'll include small section about it.

Perhaps if you've worked with C or C++ before, you include standard library headers like so:

``` c++
// Notice the use of <>
#include <vector>
```

But, when you include a header that you've created, or that is somewhere else in the project directory:

``` c++
// Notice the use of ""
#include "Vector.h"
```

This is because their semantics are slightly different.
Confusingly, the difference is implementation-dependent, but in most cases angled brackets `<>` tell the preprocessor to look in system/compiler-known headers _only_, whereas quotes `""` tell it to look relative to the file location _first_, and then fall back to the system/compiler-known headers.

System- and compiler-known headers include system-dependent headers, standard library headers, and the headers specified in the include path.

You may see people use people write code like the following:

``` c++
// Not a system or stdlib header.
#include <MyLib/util.h>
```

This means that when they compile their code, they are using a special flag to tell the toolchain to add the path to the parent of `MyLib` to the include path. This is done using the `-Ipath/to/parent` flag in compilers like `gcc`, `g++`, and `clang`.

[^1]: https://en.wikipedia.org/wiki/GNU_Compiler_Collection

[^2]: https://stackoverflow.com/questions/21515608/what-are-the-differences-between-the-c-and-c-preprocessors

[^3]: https://en.wikipedia.org/wiki/Include_guard

[^4]: https://gcc.gnu.org/onlinedocs/cpp/Pragmas.html

[^5]: https://gcc.gnu.org/onlinedocs/cpp/Pragmas.html#index-_0023pragma-once

[^6]: https://stackoverflow.com/questions/787533/is-pragma-once-a-safe-include-guard
