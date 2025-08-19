#include "Even.h"

int main()
{
    return jecpp::isEven(2) ? 0 : 1;
    // In case you have never seen the ternary operator, this is equivalent:
    //
    // ```cpp
    // if(jecpp::isEven(2)) {
    //     return 0;
    // } else {
    //     return 1;
    // }
    // ```
}
