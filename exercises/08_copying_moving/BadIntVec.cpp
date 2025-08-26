#include "BadIntVec.h"
#include <utility>

namespace jecpp {

BadIntVec::BadIntVec(size_t capacity)
    : m_things(new int[capacity])
    , m_capacity(capacity)
{
}

} // namespace jecpp

int main()
{
    jecpp::BadIntVec a(10);
    jecpp::BadIntVec b = a;
    jecpp::BadIntVec c = std::move(b);

    // NOTE: Is `a` valid anymore? What about `b` and `c`?

    jecpp::BadIntVec d(b); // XXX: Is this OK?
    jecpp::BadIntVec e(std::move(c)); // XXX: What about this?

    return 0;
}
