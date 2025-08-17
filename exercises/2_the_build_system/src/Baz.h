#pragma once

#include <cstdint>

namespace jecpp {

class Baz {
public:
    Baz() = default;

    Baz(uint64_t a, uint64_t b)
        : m_a(a)
        , m_b(b)
    {
    }

    ~Baz() = default;

    Baz(const Baz& o)
        = delete;

    Baz& operator=(const Baz& o) = delete;

    uint64_t go() const;

private:
    uint64_t m_a;
    uint64_t m_b;
};

} // namespace jecpp
