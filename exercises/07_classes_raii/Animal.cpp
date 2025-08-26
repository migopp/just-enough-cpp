#include "Animal.h"

#include <print>

// NOTE: Do not touch the following.
namespace jecpp {

Animal::Animal(const char* name)
    : m_name(name)
{
}

Animal::~Animal()
{
    std::println("Animal {} deleted", m_name);
}

void Animal::speak() const
{
    std::println("{}: <ANIMAL NOISE>", m_name);
}

Cat::Cat(const char* name, Breed breed)
    : m_name(name)
    , m_breed(breed)
{
}

Cat::~Cat()
{
    std::println("Cat {} deleted", m_name);

    // NOTE: Should exit here!
    exit(0);
}

} // namespace jecpp

int main()
{
    jecpp::Animal* myAnimal = new jecpp::Cat("Shorty", jecpp::Cat::Breed::BritishShorthair);
    myAnimal->speak();
    delete myAnimal;

    // XXX: Should not get here. Should call dtor of `Cat`, which exits with rc 0.
    return -1;
}
