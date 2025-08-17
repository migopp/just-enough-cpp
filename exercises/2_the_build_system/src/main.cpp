#include "Bar.h"
#include "Baz.h"
#include "Binky.h"
#include "Foo.h"
#include <print>

using std::println;

int main()
{
    uint64_t a = 20, b = 28;

    jecpp::Foo foo(a, b);
    jecpp::Bar bar(a, b);
    jecpp::Baz baz(a, b);
    jecpp::Binky binky(a, b);

    println("{}", foo.go());
    println("{}", bar.go());
    println("{}", baz.go());
    println("{}", binky.go());

    return 0;
}
