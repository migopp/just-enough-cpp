#include "Bar.h"
#include <cstdlib>
#include <limits>
#include <print>

namespace jecpp {

Bar::Bar(size_t capacity)
    : m_data(new int[capacity])
    , m_length(0)
    , m_capacity(capacity)
{
}

Bar::~Bar()
{
    delete[] m_data;
}

void Bar::addDataPoint(int dataPoint)
{
    // Check for resize.
    if (m_length + 1 >= m_capacity) {
        // Resize.
        size_t capacity = m_capacity << 1;
        int* data = new int[capacity];
        for (size_t idx = 0; idx < m_length; ++idx) {
            data[idx] = m_data[idx];
        }

        // Throw away old data.
        delete[] m_data;

        // Update members.
        m_data = data;
        m_capacity = capacity;
    }

    // Add data point.
    m_data[m_length++] = dataPoint;
}

void Bar::printDataPoints()
{
    for (size_t idx = 0; idx < m_length; ++idx) {
        std::println("Data[{}]: {}", idx, m_data[idx]);
    }
}

} // namespace jecpp

int main()
{
    std::println("This is going to take a second...");
    jecpp::Bar* myBar = new jecpp::Bar(5);

    for (size_t i = 0; i < std::numeric_limits<int>::max() - 2028; ++i) {
        myBar->addDataPoint(2028 + i);
    }

    delete myBar;
}
