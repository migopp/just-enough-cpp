#pragma once

namespace jecpp {

// Take a look at this class, then implement all the functionality specified.
class Foo {
public:
    /// Constructs a `Foo` with
    ///   - `m_value` == `value`,
    ///   - `m_id` == `nullptr`, and
    ///   - `m_next` == `nullptr`
    Foo(int value);

    /// Constructs a `Foo` with
    ///   - `m_value` == `value`,
    ///   - `m_id` == `id`, and
    ///   - `m_next` == `nullptr`
    Foo(int value, const char* id);

    /// Constructs a `Foo` with
    ///   - `m_value` == `value`,
    ///   - `m_id` == `id`, and
    ///   - `m_next` == `next`
    Foo(int value, const char* id, Foo* next);

    /// Constructs a `Foo` with
    ///   - `m_value` == 0,
    ///   - `m_id` == `nullptr`, and
    ///   - `m_next` == `nullptr`
    ///
    /// No member variables should be uninitialied.
    Foo() = default;

    /// Returns `m_value`.
    int getValue() const;

    /// Returns `m_id`.
    const char* getId() const;

    /// Sets `m_next` to `next`, and returns the old value of `m_next`.
    Foo* setNext(Foo* next);

    /// Prints info about `Foo` in this format;
    ///
    /// ```
    /// <m_id>: <m_value>
    /// ```
    ///
    /// For the `Foo` this is called on, and every `Foo` forward in the chain, until `m_next` is `nullptr`.
    ///
    /// Anything reasonable is OK.
    void printChain() const;

private:
    const int m_value = 0;
    const char* m_id = nullptr;
    Foo* m_next = nullptr;
};

}; // namespace jecpp
