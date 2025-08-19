#pragma once

#include <cstdint>

namespace jecpp {

class Foo {
public:
    Foo() = default;

    Foo(uint64_t a, uint64_t b)
        : m_a(a)
        , m_b(b)
    {
    }

    ~Foo() = default;

    Foo(const Foo& o)
        = delete;

    Foo& operator=(const Foo& o) = delete;

    uint64_t go() const;

private:
    uint64_t m_a;
    uint64_t m_b;
};

} // namespace jecpp
