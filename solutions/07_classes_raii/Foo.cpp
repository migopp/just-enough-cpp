#include "Foo.h"

#include <print>

namespace jecpp {

Foo::Foo(int value)
    : m_value(value)
{
}

Foo::Foo(int value, const char* id)
    : m_value(value)
    , m_id(id)
{
}

Foo::Foo(int value, const char* id, Foo* next)
    : m_value(value)
    , m_id(id)
    , m_next(next)
{
}

int Foo::getValue() const
{
    return m_value;
}

const char* Foo::getId() const
{
    return m_id;
}

Foo* Foo::setNext(Foo* next)
{
    Foo* old = m_next;
    m_next = next;
    return old;
}

void Foo::printChain() const
{
    std::println("{}: {}", m_value, m_id);

    // NOTE: Could use `this` with a const-cast, but those are terrible quality.
    Foo* next = m_next;
    while (next != nullptr) {
        if (next->m_id != nullptr) {
            std::println("{}: {}", next->m_value, next->m_id);
        } else { // NOTE: Anything reasonable is fine here.
            std::println("{}: <NO ID>", next->m_value);
        }
        next = next->m_next;
    }
}

}; // namespace jecpp

//
// NOTE: Quick test. Do not touch this section of code.
//
int main()
{
    using namespace jecpp;
    Foo f1(1);
    Foo f2(2, "f2");
    Foo f3(3, "f3", &f2);
    Foo* oldNext = f2.setNext(&f1);

    if (f1.getValue() != 1) {
        std::println("f1 value was {}, expected 1.", f1.getValue());
    }

    if (f1.getId() != nullptr) {
        std::println("f1 id was {}, expected nullptr.", f1.getId());
    }

    if (std::strcmp(f2.getId(), "f2") != 0) {
        std::println("f2 id was {}, expected \"f2\".", f2.getId());
    }

    if (oldNext != nullptr) {
        std::println("oldNext found to be {:x}, expected nullptr.", reinterpret_cast<uint64_t>(oldNext));
    }

    f3.printChain();
}
