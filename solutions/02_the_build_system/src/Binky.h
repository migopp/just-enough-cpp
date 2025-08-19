#pragma once

#include <cstdint>

namespace jecpp {

class Binky {
public:
    Binky() = default;

    Binky(uint64_t a, uint64_t b)
        : m_a(a)
        , m_b(b)
    {
    }

    ~Binky() = default;

    Binky(const Binky& o)
        = delete;

    Binky& operator=(const Binky& o) = delete;

    uint64_t go() const;

private:
    uint64_t m_a;
    uint64_t m_b;
};

} // namespace jecpp
