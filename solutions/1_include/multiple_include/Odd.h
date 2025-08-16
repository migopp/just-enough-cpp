#pragma once

#include "Even.h"

namespace jecpp {

bool isOdd(int n)
{
    return !isEven(n);
}

} // namespace jecpp
