#include "time.hpp"

#include <numeric>

using music::time;

time::time (unsigned n, unsigned d):
	numerator{n}, denominator{d}
{}



time time::operator+(const time& other)
{
    time result;

    result.denominator = std::lcm(other.denominator, this->denominator);
    result.numerator = (other.numerator*(result.denominator/other.denominator) + this->numerator*(result.denominator/this->denominator));

    return result;
}

bool time::operator==(const time& other)
{
   return (other.denominator == this->denominator)&&(other.numerator, this->numerator);
}

bool time::operator==(const time& other)
{
   return !(*this==other);
}