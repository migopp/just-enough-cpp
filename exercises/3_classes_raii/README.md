# Classes and RAII

**Contents**

In C, users can define custom data types through use of the `struct` keyword. Resource management is done manually, usually via the use of `make_X()` and `destroy_X()` functions or some equivalent.

C++ provides alternative, more sophisticated ways of creating custom data types, as well as ways to automagically manage resources required of these types. This section will cover the basic features of C++'s classes and structs without particular emphasis on inheritance or polymorphism.

> [!WARNING]
> This is the first section that I would not recommend anyone skip. Unless you are quite confident in access specifiers, member functions, the mechanics of RAII (including constructors, destructors, copy constructors, copy assignment, and move constructors), initializer lists, and the mechanics of creating objects.

## C Structs

Let's start with what you already know. C has the `struct` keyword that allows users to define custom data types:

``` c
struct Point {
    int x;
    int y;
};
```

And you can create a `Point` object like so:

``` c
struct Point p = {2, 8};
```

For this reason, oftentimes you'll see struct definitions `typedef`'d like so:

``` c
typedef struct Point_ {
    int x;
    int y;
} Point;
```

So that the extra `struct` annotation is not needed:

``` c
Point p = {2, 8};
```

You can make a function that operates on a `struct` like so, by passing in a pointer an object of that type:

``` c
void point_add_to_x_coordinate(Point *p, int delta)
{
    // ...
}
```

## Defining Custom Types in C++

In C++, we have two options for declaring custom types. As in C, there are structs, but there are also classes. These ultimately achieve very similar things, but their semantics are different.

We will discuss their specific uses and differences soon enough, but for now, know that they are declared and their types are referenced like so:

``` c++
class Point {
    int m_x;
    int m_y;
};

Point p;
```

``` c++
struct Point {
    int x;
    int y;
};

Point p;
```

Notice how in the last case, `struct Point` is not longer required to reference the `Point` struct.

## History

C++ actually started as a transpiler for a dialect of C with classes. That is, you would write C++ code as C with classes, and it would be transformed into vanilla C. Member variables would be stored in a struct, and methods were nothing more than a function that passes in a pointer to a variable with the struct type.

So, it's reasonable that a class like so:

``` c++
class Point {
public:
    int m_x;
    int m_y;
    
    void modifyXCoord(int delta);
    void modifyYCoord(int delta);
};
```

Would transpile into something like:

``` c
struct _class_Point {
    int m_x;
    int m_y;
};

void _class_Point_modifyXCoord(struct _class_Point* p, int delta);
void _class_Point_modifyYCoord(struct _class_Point* p, int delta);
```

It's evolved a fair bit since then and become more sophisticated, but the same idea remains. A class is just a way to abstract away raw data and processes that work on that data.

## Member Variables

_Member variables_ are the data associated with a `struct` or `class`.

For example, take the point class mentioned above:

``` c++
class Point {
    int m_x;
    int m_y;
};
```

It has two member variables: `m_x` of type `int`, and `m_y` of type `int`.

## Member Functions

_Member functions_ are known as methods in other languages. They are the operations that operate on `struct` or `class` data.

``` c++
struct Point {
    int x;
    int y;
    
    // Notice that `struct` types can also have member functions!
    double distanceTo(Point *other);
};
```

## Access Modifiers

You may have questioned the use of the `public:` and `private:` labels of code above: 

``` c++
class MyClass {
public:
// ...

private:
// ...
};
```

These are _access modifiers_. They specify which entities are able to access the class members that follow. Unlike a language like Java, the access modifier of members are not specified individually, but in blocks.

- `public`: Everything can access.
- `protected`: This class and subclasses can access.
- `private`: This class _only_ can access.


These are used for encapsulating data. If a member function is not intended to be part of the user-facing API, then you should mark it in either `protected` or `private` access blocks.

By default, members of `class` are `private`, while members of `struct` are `public` for C compatability. This is the only functional difference between the two constructs.

## RAII, Constructors, and Destructors

Now we get to the important part.

You've seen this term thrown around a few times now: _resource acquisition is initialization_. It simply a principle that states when you initialize an object, it's resources should be inherently acquired. Any allocations it requires to function should be made, and member variables that must be set should be set: all without the programmer doing it conciously.

In C, there is the common pattern of:

``` c
struct Thing {
    int a;
    double *sv;
    // ...
};

// In code using `Thing`:
struct Thing *t = malloc(sizeof(Thing));
t.a = 0;
double *ourDouble = malloc(sizeof(double));
*ourDouble = 2.8;
t.sv = ourDouble;
// And so on, initializing all the fields.

// ...

// Then, once we are done with `t`:
free(t.sv);
free(t):
```

This is problematic because it's very verbose, and there's a lot of code to forget to write. If you forget to initialize a field, you get junk in its place. If you forget one of the `free()` calls at the end, then you have a memory leak. What if you encounter an error? In every case, you have to remember to initialize and deinitialize everything.

To aid in this effort, people create specialized functions to make and destroy objects of a certain type. That way, you only need to write two lines of code per use:

``` c
struct Thing { ... };

struct Thing *make_thing(int a, double *sv ...);
void destroy_thing(struct Thing* t); // This also frees the ptr `t->sv`.

// In code using `Thing`:
struct Thing *t = make_thing(0, { double *d = malloc(sizeof(double)); d = 2.8; d; }, ...);

// ...

// Then, once we are done with `t`:
destroy_thing(t);
```

This is better, but it still introduces the possibility of error. The programmer may simply _forget_ to call `destroy_thing()` in all cases.

Here's an idea: instead of manually calling `make_thing()` and `destroy_thing()`, what if we had the language automatically call `make_thing()` when we create a variable of type `Thing`, and automatically call `destroy_thing()` when that variable goes out of scope?

This is the idea of RAII. The former is what we call a _constructor_, and the latter is a _destructor_. Creating a `Thing` and acquiring the resources for it are inherently coupled, so why not couple their operations into one? The same is true for destroying a `Thing` and releasing the resources for it.

Hopefully you are able to see why this idea is so useful. We can now jump into the mechanics of it in C++.

### Constructors

The syntax for a constructor is:

``` c++
class Point {
public:
    Point(int x, int y)
        // ^- Parameter list
        : m_x(x) // Initializer list
        , m_y(y)
    {
        // Constructor body
    }
    
private:
    int m_x;
    int m_y;
};
```

If you want all of your fields default-constructed, use `= defualt`.

``` c++
class MyClass {
public:
    MyClass() = default;
};
```

Then, the compiler will generate a constructor for you, which generally leads to faster code than creating an empty user-defined constructor:

``` c++
class MyClass {
public:
    MyClass() {}
};
```

#### Uninitialized Members

Say we decided we didn't want to write all the code from above.

``` c++
class LazyPoint {
public:
    int m_x;
    int m_y;
};
```

If we made a `LazyPoint`, like so:

``` c++
LazyPoint l;
```

The values of `l.m_x` and `l.m_y` would be uninitialized. This means they retain the garbage value left behind in the piece of memory that `l` occupies. If you are lucky, this is `0`, but it's also likely to be some random garbage value like `1337`.

#### Initializer Lists

You are likely aware of the parameter list and the constructor body, but you may have not seen an initializer list before.

In C++, you can use either of these forms after the constructor signature to initialize member variables:

``` c++
MyClass(...): m_a(a), m_b(b), ... {}
```

``` c++
MyClass(...): m_a{a}, m_b{b}, ... {}
```

It might be first nature to do something like this:

``` c++
class Point {
public:
    Point(int x, int y) {
        m_x = x;
        m_y = y;
    }

private:
    int m_x;
    int m_y;
};
```

This is, in fact, generally less efficient than using the initializer list. You'll learn more about this as you use C++ more, but before the constructor body is called, all member variables are default-constructed if they are not initialized. Though, in _this_ case, built-in types like `int` don't have constructors, so they just remain uninitialized.

Suppose we had the following class:

``` c++
class MyClass {
public:
    MyClass(std::vector<int> vec) {
        m_vec = vec;
    }
    
private:
    // This is just a dynamic array of `int`s.
    std::vector<int> m_vec;
};
```

Then, before the body of the constructor runs, `m_vec` is default-constructed. Then, we assign `vec` to `m_vec` anyway, meaning we didn't _need_ to default-construct `m_vec`. It was a waste.

In fact, the above code is quite inefficient for a few other reasons we will touch on soon, but keep in mind that you should prioritize initializing your member variables outside of the constructor body.

Another reason you would want to use an initializer list over assigning to the member variable in the constructor body is if the member variable is marked `const` (we will talk about this keyword soon if you are rusty, but it essentially means that after the variable is initialized, the value cannot change).

``` c++
class ConstPoint {
public:
    ConstPoint(int x, int y) {
        // XXX: This does not compile.
        m_x = x;
        m_y = y;
    }

private:
    const int m_x;
    const int m_y;
};
```

This is because `m_x = x` in the constructor body is seen as an _assignment_, not an initialization.

One more quirk about initializer lists is that regardless of the order you place the initializations in, they occur in the order in which the member variables were declared:

``` c++
class Binky {
public:
    Binky(int x) : m_foo(x), bar(m_foo - 2) {}

private:
    int m_bar;
    int m_foo;
};
```

Here, `m_foo` is declared _after_ `m_bar`, so the initializer `bar(m_foo - 2)` runs first, but `m_foo` was not initialized to `x` yet.

#### Default Member Initialization

There's another mechanism by which you can initialize member variables called _default member initialization_.

``` c++
class Point {
public:
    Point(int x, int y) : m_x(x), m_y(y) {}

private:
    int m_x = 0; // (1)
    int m_y{0};  // (2)
};
```

Forms `(1)` and `(2)` are both OK. The above class definition achieves the same results as:

``` c++
class Point {
public:
    Point() : m_x(0), m_y(0) {}
    // Or, `Point() = default;`

    Point(int x, int y) : m_x(x), m_y(y) {}

private:
    int m_x;
    int m_y;
};
```

Often, the default member initialization style is preferred out of simplicity, but either style of initialization is fine. _Just make sure that member variables are initialized, no matter what._

### Destructors

Destructors have similar syntax to constructors:

``` c++
class Point {
public:
    // ...
    
    ~Point() {
        // Destructor body
    }
    
private:
    // ...
};
```

There's not much to them. If you don't make any special allocations (using `new` or `malloc`, the latter you should _never_ use in C++), then you likely don't need a special destructor. Some other special cases might be if you have open handles (to things like files, sockets, or database connections); then, you may also need to define your own constructor. In the cases that you _don't_ need a special destructor, you should use `= default` for efficiency.

``` c++
class Point {
public:
    Point() = default;
    
    Point(int x, int y) : m_x(x), m_y(y) {}
    
    // Default destructor is fine.
    ~Point() = default;
    
private:
    int m_x = 0;
    int m_y = 0;
};
```

Let's say you had a class which allocated a dynamic array of `int`s on the heap. In this case, you'd want to free up the resource in the destructor.

``` c++
class PointList {
public:
    PointList(size_t sz)
        : m_xCoords(new std::vector<int>(sz))
        , m_yCoords(new std::vector<int>(sz)) {}
        
    ~PointList() {
        delete m_xCoords;
        delete m_yCoords;
    }

private:
    std::vector<int> m_xCoords;
    std::vector<int> m_yCoords;
};
```

Don't worry if you don't understand `new` and `delete` yet, just remember that you need a user-defined destructor (i.e., not `default`) if you need special functionality when your object is deleted, that is, when you manage a raw resource like a pointer into memory or a handle.

## Declaration and Definition

As we discussed in the section on the `include` directive, declarations should be placed in header files, and definitions should be placed in source files. The same applies to classes and structs.

``` c++
// Foo.h

class Foo {
public:
    // Unless you are _only_ using the initializer list, and your constructor body is empty, you should _not_ define your constructor in a header file.
    Foo();
    // i.e.,
    //
    // OK:
    // Foo() : m_a(2), m_b(8), ... {}
    //
    // BAD:
    // Foo() : m_a(2), m_b(8), ... { std::println("Hello, world!"); ... }
    
    // Declare a member function here.
    void tomfoolery(bool binky);

private:
    int m_a;
    int m_b;
    int m_c;
    // ...
}

// Foo.cpp
#include "Foo.h"

Foo::Foo()
    // Initializer lists here are OK!
    : m_a(6)
    , m_b(7)
    , m_c(8)
{
    std::println("`Foo` constructor called!");
}

void Foo::tomfoolery(bool binky)
{
    // ...
}
```

`::` is the scope resolution modifier; it allows us to have a function with the signature `tomfoolery(bool binky)` in global scope (return type is not part of the function signature in C++), and not confuse it with the member function of `Foo` which goes by the same signature. Think of `Foo::` as meaning `Foo`'s.

## `new` and `delete`

## Copying

### Copy Constructor

### Copy Assignment

## Moving

## Const Member Functions

## Static Member Functions

## Basic Inheritance
