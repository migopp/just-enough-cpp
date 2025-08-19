#include "Even.h"

namespace jecpp {

bool isSuperEven(int n)
{
    return isEven(n) && isEven(n + 2);
}

} // namespace jecpp
