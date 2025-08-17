#pragma once

#include <cstdint>

namespace jecpp {

class Bar {
public:
    Bar() = default;

    Bar(uint64_t a, uint64_t b)
        : m_a(a)
        , m_b(b)
    {
    }

    ~Bar() = default;

    Bar(const Bar& o)
        = delete;

    Bar& operator=(const Bar& o) = delete;

    uint64_t go() const;

private:
    uint64_t m_a;
    uint64_t m_b;
};

} // namespace jecpp
