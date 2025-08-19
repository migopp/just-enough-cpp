# Classes and RAII

**Contents**
1. [C Structs](#c-structs)
2. [Defining Custom Types in C++](#defining-custom-types-in-c)
3. [History](#history)
4. [Member Variables](#member-variables)
5. [Member Functions](#member-functions)
6. [Access Modifiers](#access-modifiers)
7. [RAII, Constructors, and Destructors](#raii-constructors-and-destructors)
   - [Constructors](#constructors)
     - [Initializer Lists](#initializer-lists)
     - [Default Member Initialization](#default-member-initialization)
   - [Destructors](#destructors)
8. [Declaration and Definition](#declaration-and-definition)
9. [`new` and `delete`](#new-and-delete)
   - [`new[]` and `delete[]`](#new-and-delete-1)
   - [Mixing `new` and `free`](#mixing-new-and-free)
   - [Calling `delete`, `delete[]` On Casted Pointer](#calling-delete-delete-on-casted-pointer)
10. [`this`](#this)
11. [Const Member Functions](#const-member-functions)
12. [Static Member Functions](#static-member-functions)
13. [Basic Inheritance](#basic-inheritance)
    - [Access Modifiers](#access-modifiers-1)
    - [Virtual Functions](#virtual-functions)
    - [Final](#final)
    - [Slicing](#slicing)
    - [The Classic Virtual Destructor](#the-classic-virtual-destructor)

In C, users can define custom data types through use of the `struct` keyword. Resource management is done manually, usually via the use of `make_X()` and `destroy_X()` functions or some equivalent.

C++ provides alternative, more sophisticated ways of creating custom data types, as well as ways to automagically manage resources required of these types. This section will cover the basic features of C++'s classes and structs without particular emphasis on inheritance or polymorphism.

> [!WARNING]
> This is the first section that I would not recommend anyone skip.
>
> Unless you are quite confident in access specifiers, member functions, the mechanics of RAII (including constructors, destructors, copy constructors, copy assignment, and move constructors), initializer lists, and the mechanics of creating objects, you will likely learn something important from this section.

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

If all of your member fields are default-constructable (i.e., have `T()` for their type of `T`, use `= defualt`.

``` c++
class MyClass {
public:
    MyClass() = default;
    
    // ...
};
```

Then, the compiler will generate a constructor for you, which generally leads to faster code than creating an empty user-defined constructor:

``` c++
class MyClass {
public:
    MyClass() {}
};
```

However, it may also leave members unitialized if they are built-in types (e.g., `int` or `int*`).

#### Uninitialized Members

Say we decided we didn't want to write all the code from above.

``` c++
class LazyPoint {
public:
    LazyPoint() = default;

private:
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
    Point(int x, int y)
    {
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
    MyClass(std::vector<int> vec)
    {
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
    ConstPoint(int x, int y)
    {
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

    Point(int x, int y) : m_x(x), m_y(y) {}

private:
    int m_x;
    int m_y;
};
```

Also, if you have both a default initialization for a member and an initializer list entry, the initializer list entry overrides the default initialization. That is, if one has:

``` c++
class Foo {
public:
    Foo(int x) : m_x(x) {}
    
private:
    int m_x = 0;
};
```

Then, `Foo foo(3);` results in `foo.m_x` being 3, not 0.

Often, the default member initialization style is preferred to writing a default constructor with an initializer list because of simplicity, but either style of initialization is fine. _Just make sure that member variables are initialized, no matter what._

### Destructors

Destructors have similar syntax to constructors:

``` c++
class Point {
public:
    // ...
    
    ~Point()
    {
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
        
    ~PointList()
    {
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

From now on, I am going to write everything (including implmentation) in the class definition itself, rather that in a separate `.cpp` file as above. This is _bad_, don't do it in real code. I am only doing it for brevity.

## `new` and `delete`

I spoke about `new` and `delete` above; they simply handle the process of managing pointers into the heap. In C, you were used to doing this with `malloc` and `free`, but they have the fatal flaw of not initializing/deinitializing the memory that they provide.

``` c++
class Thing {
 // ...
};

Thing *t = malloc(sizeof(Thing));
```

In the above example, any fields of `t` are undefined because `Thing`'s constructor was not called. We could follow every `malloc` call with a call to the constructor:

``` c++
Thing *t = malloc(sizeof(Thing));
*t = Thing(...);
```

And then every `free` proceeds from a call to the destructor:

``` c++
t->~Thing();
free(t);
```

But, we have a shorthand for these; it's `new`:

``` c++
// (1) and (2) are _roughly_ equivalent.

Thing *t = new Thing(...);        // (1)

Thing *t = malloc(sizeof(Thing)); // (2)
*t = Thing(...);
```

and `delete`:

``` c++
// (1) and (2) are _roughly_ equivalent.

Delete t;    // (1)

t->~Thing(); // (2)
free(t);
```

In fact, these are slight oversimplifications because `new` and `delete` throw exceptions on failure (do not worry about exceptions, they do not come up in this class) and are more efficient than the "equivalent" code provided. These are just simple models that get you 95% understanding `new` and `delete`.

### `new[]` and `delete[]`

What if you want to allocate an array on the heap? There are specialized memory management operators `new[]` and `delete[]` for this:

``` c++
int arr[] = new int[5]; // An array of 5 ints on the heap.

delete[] arr;
```

`new[]` acts as you would expect: it allocates enough space for all elements, then calls the constructor on each space. `delete[]` calls the destructor on each element, and then deallocates the entire lot.

### Mixing `new` and `free`

You _cannot_ allocated memory with `new` and then free it with `free`. This is undefined behavior[^1].

Always match a `new` with a `delete`, a `new[]` with a `delete[]`, and a `malloc` with a `free`.

### Calling `delete`, `delete[]` On Casted Pointer

Another important behavior that is undefined is calling `new` or `new[]`, then casting the result and calling `delete` or `delete[]` on it[^2].

``` c++
Thing *t = new Thing(...);
Binky *b = (Binky *)t; // This kind of cast is quite bad.
delete b;
```

This is because the wrong destructor for `Binky` will be invoked instead of `Thing`.

## `this`

Within the body of a class, `this` is a keyword that contains a pointer to the instance of that class.

``` c++
class Bar {
public:
    void tomfoolery()
    {
        std::println("{}", this->m_baz); // Prints `5`.
    }

private:
    int m_baz = 5;
};
```

## Const Member Functions

You can annotate that certain member functions will _not_ modify an object's data member variables with the `const` keyword like so:

``` c++
class Bar {
public:
    Bar(int x) : m_x(x) {};
    
    void print() const
    {
        // Does not modify any of `this` member variables, so `const` is appropriate.
        std::println("{}", m_x);
    }
    
private:
    int m_x = 0
};
```

Note that `const` becomes part of the method signature, so it needs to be specified again in the `.cpp` file implmentation:

``` c++
// Bar.cpp

void Bar::print() const
{
    // ...
}
```

## Static Member Functions

You've seen `static` before. Used at the global level, it specifies internal linkage, meaning that the symbol is not visible from other translation units (source files). Used within a function body, it gives the variable static storage duration; that is, it's value is preserved across function calls for the lifetime of the program.

C++ introduces yet another distinct meaning it can have. When you mark a member variable or function with the `static` modifier, it declares that the variable or function belongs to the _class_ rather than the object or instance.

For example:

``` c++
// Baz.h
class Baz {
public:
    static int constructions;
    
    Baz();
};

// Baz.cpp
int Baz::constructions = 0;

Baz::Baz()
{
    ++constructions;
}
```

Then, if we construct a few `Baz` objects and check the value of `Baz::constructions`, we will see it updates:

``` c++
Baz b;
Baz c;
std::println("{}", Baz::constructions); // 2
```

And for member functions:

``` c++
// Baz.h
class Baz {
public:
    // ...
    
    static void tomfoolery();
};

// Baz.cpp
void Baz::tomfoolery()
{
    std::println("Tomfoolery.");
}
```

Then, we can call it without instantiating an object of type `Baz`:

``` c++
Baz::tomfoolery();
```

## Basic Inheritance

Inheritance gets quite complicated, and you really should not need to touch it a ton in this class, so I'll be quite brief. If you want to learn more, you should find any reputable OOP C++ textbook and read through the sections about inheritance to iron out the quirks. I think that _C++ Primer 5E_ is quite good.

To mark a class as _derived_ from another use the `: <Access Modifier> BaseClass` syntax, like so:

``` c++
class Base { ...  };

class Derived : public Base { ... };
```

### Access Modifiers

The access modifier can be any of `public`, `protected`, or `private`. All it does is set the default level of access for the members of `Base`.

- `public`: Base's `public` fields are marked `public` in the derived. `protected` members are still `protected`. `private` members of the base are not inherited.
- `protected`: Base's `public` and `protected` fields are marked `protected` in the derived. `private` members of the base are not inherited.
- `private`: Base's `public` and `protected` fields are marked `private` in the derived. `private` members of the base are not inherited.

Derived classes inherit the members of the base class.

### Virtual Functions

You can get polymorphic behavior by marking a function as `virtual` in the base class. This tells the compiler to use dynamic dispatch (i.e., generate a virtual table (vtable) look up for the function) so that the implementation of the derived class is used over the base class.

A member function is marked virtual like so:

``` c++
class Foo {
public:
    virtual void tomfoolery() const
    {
        std::println("Foo");
    }
};
```

And, you can override it in a derived class by marking it as an `override`:

``` c++
class Bar : public Foo {
public:
    void tomfoolery() const override
    {
        std::println("Bar");
    }
};
```

You can declare a virtual function as pure by adding an `= 0`. It makes it so that the class has no implementation of that member function, making it impossible to instantiate an object of that class. We call such a class _abstract_, or an interface.

``` c++
class AbstractFoo {
public:
    virtual void tomfoolery() const = 0;
};

AbstractFoo af; // This is illegal.
```

Derived classes _must_ override pure functions. It is not optional, unlike overriding `virtual` functions with implementations.


### Final

You can mark a virtual member function `final` to tell the compiler that it should not be overriden by any derived class.

``` c++
class Baz : public Bar {
public:
    void tomfoolery() const override final
    {
        std::println("Baz");
    }
};
```

You can also mark a class as final to tell the compiler that the class should not be inherited from.

``` c++
class Binky final : public Baz {
    // ...
};
```

### Slicing

Note that when you perform an assignment like the following:

``` c++
Base b = Derived(...);
```

If `b` has any members marked `virtual`, then they will call the implementation defined in `Base`. This phenomenon is described in the standard and commonly referenced as _slicing_; as in _slicing off_ the implementation of `Derived`. The details of how this works are irrelevant for now, but essentially the vtable inside of `b` gets set to that of `Base`. If you would like to assign a `Derived` object to a `Base`, then use pointers:

``` c++
Base *b = new Derived(...);

// Or,
Derived d;
Base *b = &d;
```

Both of these options retain the vtable of `Derived`.

### The Classic Virtual Destructor

One special function you may need to make `virtual` is your destructor. This is not always intuitive, so I'll explain when it is needed quickly.

Suppose that you have a `Base` and `Derived`, for which the derived has a specialized destructor:

``` c++
class Base {
public:
    // ...
    
    ~Base() { ... }
};

class Derived : public Base {
public:
    // ...
    
    ~Derived() { ... }
};
```

Then, if you have:

``` c++
Base *thing = new Derived(...);
```

And you want to free the memory for `thing`:

``` c++
delete thing;
```

This will call the destructor for `Base`, not `Derived`! This is because even though we have marked `thing` as a pointer, and `thing` is _not_ sliced, we have not said that the destructor is virtual.

For this, the solution is simple. Just mark the destructor as `virtual` in `Base`.

``` c++
class Base {
public:
    // ...
    
    virtual ~Base() { ... }
};
```

[^1]: https://stackoverflow.com/questions/22406278/is-it-safe-to-free-memory-allocated-by-new

[^2]: https://stackoverflow.com/questions/2140319/can-i-new-then-cast-the-pointer-then-delete-safely-with-built-in-types-in
