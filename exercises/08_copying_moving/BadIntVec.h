#pragma once

#include <cstddef>

namespace jecpp {

// NOTE: Implement each of the special member functions of this class.
class BadIntVec {
public:
    /// Default constructor.
    BadIntVec() = default;

    /// Constructor that specifies the initial capacity.
    BadIntVec(size_t capacity);

    /// Destructor.
    ~BadIntVec();

    /// Copy constructor.
    BadIntVec(const BadIntVec& o);

    /// Copy assignment operator.
    BadIntVec& operator=(const BadIntVec& o);

    /// Move constructor.
    BadIntVec(BadIntVec&& o);

    /// Move assignment operator.
    BadIntVec& operator=(BadIntVec&& o);

    // NOTE: Don't worry about the practicals here. No need to implement any vector API operations.

private:
    /// Underlying collection.
    int* m_things = nullptr;

    /// Number of elements in the vector.
    size_t m_size = 0;

    /// Size of `m_things`.
    size_t m_capacity = 0;
};

class Wrapper {
public:
    /// Default constructor.
    Wrapper() = default;

    // XXX: Which other special members are needed to maintain `m_biv`?
    //
    // Hint: Does one of our rules apply?

private:
    BadIntVec m_biv = {};
};

} // namespace jecpp
