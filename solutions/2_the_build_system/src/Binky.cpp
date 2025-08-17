#include "Binky.h"

uint64_t jecpp::Binky::go() const
{
    return m_b == 0 ? 0 : m_a / m_b;
}
