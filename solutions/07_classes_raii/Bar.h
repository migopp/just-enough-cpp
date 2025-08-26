#pragma once

#include <cstddef>

namespace jecpp {

class Bar {
public:
    /// Default constructor.
    Bar() = default;

    /// Constructor specifying the size of the array held in `Bar`.
    Bar(size_t length);

    /// Destructor.
    ~Bar();

    /// Adds a data point to collection of data points.
    void addDataPoint(int dataPoint);

    /// Prints all available data points.
    void printDataPoints();

private:
    /// The data points.
    int* m_data = nullptr;

    /// The number of data points in `m_data`.
    size_t m_length = 0;

    /// The capacity of `m_data` (how many elements it has the _potential_ to hold).
    size_t m_capacity = 0;
};

} // namespace jecpp
