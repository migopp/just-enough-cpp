# Copying and Moving

**Contents**
1. [Copying](#copying)
    - [Copy Constructor](#copy-constructor)
    - [Copy Assignment Operator](#copy-assignment-operator)
2. [Moving](#moving)

## Copying

Suppose you have some class:

``` c++
class Foo {
public:
    Foo(size_t sz) : m_dataArr(new int[sz]), m_dataArrSz(sz) {}
    
    ~Foo()
    {
        delete[] m_dataArr;
    }

    int *m_dataArr;
    size_t m_dataArrSz;
};
```

And you wanted to pass an object into the function by value:

``` c++
void bar(Foo barFoo)
{
    // ...
    
    barFoo.m_dataArr[0] = -1;
};

int main()
{
    Foo mainFoo(2);
    mainFoo.m_dataArr[0] = 1;
    bar(mainFoo);
    std::println("{}", mainFoo.m_dataArr[0]);
}
```

What should happen to `mainFoo` as it is passed into `bar`? After the call to `bar`, `mainFoo.m_dataArr[0]` should be 1 because `bar` does not take in a pointer or reference to a `Foo`. As you undertand of primitive or built-in types like `int`, a copy is made. The same thing is true of user-defined data types that are passed by value.

A copy is also made when an object is assigned like so:

``` c++
Thing t1(2);
Thing t2 = t1; // A copy of `t1` is made.
```

This should make sense given your knowledge of C. The same thing happens when working with a struct by value.

But, how does it get copied? By default, the bytes of the underlying object are `memcpy`'d into the space allocated for the new object. This is often fine (i.e., in most cases a raw resource is not managed by the class); but, what if that is _not_ our ideal or expected behavior?

Take, for example, `Foo`, which allocates and manages an array on the heap. If we make a copy for the invocation of `bar`, at the end of scope, the `barFoo` will destruct, freeing `mainFoo.m_dataArr`, because the pointer to it was copied when `barFoo` was constructed. This means that in the line where we print `mainFoo.m_dataArr[0]`, we are accessing relinquished memory!

Fortunately, we have mechanisms to control how copies are made.

### Copy Constructor

One of these mechanisms is called the _copy constructor_, which is invoked when an object is made out of an object of the same type. This is the signature:

``` c++
class Foo {
public:
    // ...
    
    Foo(const Foo& o);
    
private:
    // ...
};
```

The parameter `const Foo& o` is a _const lvalue reference_. Recall that it is essentially just a non-null pointer to a `Foo`, which cannot be modified.

The copy constructor is what is called, for example, when `barFoo` is constructed. We can also forcibly invoke the copy constructor like so:

``` c++
Foo f(5);
Foo b(f); // Pass in `f` to `Foo` constructor -> calls copy constructor.
```

By default, the compiler generates a copy constructor for each class you define, unless you overwrite it with a new one, or you tell it not to generate one with an `= delete` like so:

``` c++
class Foo {
public:
    // ...
    
    Foo(const Foo& o) = delete;
    
private:
    // ...
};
```

Using `= default` tells the compiler to generate a default copy constructor, which has the `memcpy` effect discussed above.

In the case of `Foo`, a correct copy constructor implementation might look like:

``` c++
class Foo {
public:
    // ...
    
    Foo(const Foo& o)
        : m_dataArr(new int[o.m_dataArrSz])
        , m_dataArrSz(o.m_dataArrSz)
    {
        for (size_t i = 0; i < m_dataArrSz; ++i) {
            m_dataArr[i] = o.m_dataArr[i];
        }
    }
    
private:
    // ...

};
```

Notice that the use of an initializer list is allowed. A copy constructor is a _constructor_, after all.

### Copy Assignment Operator

This is the code invoked when something like the following happens:

``` c++
Foo f(4):
Foo b = f;
```

The _assignment_ of `f` into `b` is a _copy assignment_, and the `=` which takes a `Foo` on the right-hand side is a _copy assignment operator_. I am trying to be super clear here so that the syntax of the operator makes sense:

``` c++
class Foo {
public:
    // ...
    
    Foo& operator=(const Foo& o);
    
private:
    // ...
};
```

One might first look at this and be confused why it returns a `Foo&`. This is simply because in C++, `a = b` is seen as expression returning the value of `a` after the assignment. Think of the following usecase:

``` c++
int result;
if (result = didItWork()) {
    // ...
} else {
    // ...
}
```

Then, we can quickly save the result to a variable (for use in the `else` branche), and check if we had any intermediate failure (with the `if` condition).

Now, you might first implement a copy assignment operator like this:

``` c++
class Foo {
public:
    // ...
    
    Foo& operator=(const Foo& o)
    {
        // Initialize members.
        m_dataArrSz = o.m_dataArrSz;
        m_dataArr = new int[m_dataArrSz];

        // Like copy constructor above.
        for (size_t i = 0; i < m_dataArrSz; ++i) {
            m_dataArr[i] = o.m_dataArr[i];
        }
        
        // Must return a reference, so `*(Foo *)` -> `Foo` -> `Foo&` (implicit).
        return *this;
    }

private:
    // ...
};
```

But, this misses something quite important. Consider that `b` could have been initialized before it was assigned to `f`:

``` c++
Foo f(4);
Foo b(2);
b = f;
```

Then, the code, as written, does not properly clean up the old resources of `b`. The memory is leaked. So, we should take care to clean up first:

``` c++
class Foo {
public:
    // ...
    
    Foo& operator=(const Foo& o)
    {
        // Clean up.
        delete[] m_dataArr;

        // Initialize members.
        m_dataArrSz = o.m_dataArrSz;
        m_dataArr = new int[m_dataArrSz];

        // Like copy constructor above.
        for (size_t i = 0; i < m_dataArrSz; ++i) {
            m_dataArr[i] = o.m_dataArr[i];
        }
        
        // Must return a reference, so `*(Foo *)` -> `Foo` -> `Foo&` (implicit).
        return *this;
    }

private:
    // ...
};
```

This is much better. One more detail to consider. The following is also possible:

``` c++
Foo f(4);
f = f;
```

In fact, as is, this will brick our code becuase by calling `delete[]` on the array of `this`, we also delete the array of `o`. To fix this, let's just check if `this` points to the same thing that `o` references.

``` c++
class Foo {
public:
    // ...
    
    Foo& operator=(const Foo& o)
    {
        // Check if self-assignment attempt.
        if (this == &o) { // Getting the address of a reference gives you the address of the original object.
            return;
        }
    
        // Clean up.
        delete[] m_dataArr;

        // Initialize members.
        m_dataArrSz = o.m_dataArrSz;
        m_dataArr = new int[m_dataArrSz];

        // Like copy constructor above.
        for (size_t i = 0; i < m_dataArrSz; ++i) {
            m_dataArr[i] = o.m_dataArr[i];
        }
        
        // Must return a reference, so `*(Foo *)` -> `Foo` -> `Foo&` (implicit).
        return *this;
    }

private:
    // ...
};
```

## Moving

Recall the process of taking a copy of a `Foo` from the above section. It can be expensive, especially if `m_dataArrSz` is large. What if we don't need the original object anymore?

``` c++
Foo mainFoo(100000);
bar(mainFoo);

// `mainFoo` is never used or needed again.
```

Then, it's a waste to _copy_ `mainFoo`, right? After all, it's never used again. A `memcpy` into `barFoo` would be enough, and much more efficient. We call this process _moving_ the contents of `mainFoo`. There is a special constructor for moving with the following syntax:

``` c++
class Foo {
public:
    // ...
    
    Foo(Foo&& o);
    
private:
    // ...
};
```

The `Foo&&` is an _rvalue reference_. This is a special type of reference used for temporary values, or values that can be moved from. In this case, it is a special marker that `o` references a value that we can steal resources from safely, because it will be discarded.

One correct implementation of a move constructor for `Foo` might be:

``` c++
class Foo {
public:
    // ...
    
    Foo(Foo&& o)
        : m_dataArr(o.m_dataArr)
        , m_dataArrSz(o.m_dataArrSz)
    {
        o.m_dataArr = nullptr;
        o.m_dataArrSz = 0;
    }
    
private:
    // ...
};
```

Notice again that this is constructor, so the initializer list syntax is allowed and quite convenient.

To tell the compiler that you would like to _move_ an object, you can just cast it to an rvalue reference before giving `mainFoo` to `bar`:

``` c++
Foo mainFoo(100000);
bar(static_cast<Foo&&>(mainFoo));
```

This is done more cleanly by the `std::move()` function, which is available in the `<utility>` header, even in freestanding environments (like this class). `std::move()` works even when passing in an lvalue or rvalue reference.

``` c++
Foo mainFoo(100000);
bar(std::move(mainFoo));
```

One more quirk to note is that you _cannot_ move from a const object.

``` c++
const Foo mainFoo(100000);
bar(std::move(mainFoo)); // This will still call the copy constructor, since `std::move(mainFoo)` is of type `const Foo&&`, not `Foo&&`.
```
