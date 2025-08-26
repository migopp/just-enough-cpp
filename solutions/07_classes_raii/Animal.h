#pragma once

namespace jecpp {

/// <ANIMAL NOISE>.
class Animal {
public:
    /// Default constructor.
    Animal() = default;

    /// Constructor that allows us to specify the name.
    Animal(const char* name);

    /// Destructor.
    virtual ~Animal();

    /// Animal speaks.
    virtual void speak() const;

protected:
    const char* m_name = nullptr;
};

/// ニャン.
class Cat : public Animal {
public:
    enum class Breed {
        Tabby,
        BritishShorthair,
        Sphynx
    };

    /// Constructor allows us to specify name and breed.
    Cat(const char* name, Breed breed);

    /// Destructor.
    ~Cat();

protected:
    Breed m_breed;
};

} // namespace jecpp
