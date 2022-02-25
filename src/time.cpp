#include "time.hpp"

#include <numeric>

using music::time;

time::time (unsigned n, unsigned d):
	numerator{n}, denominator{d}
{}

music::time time::operator+(const time& other)
{
   unsigned den = std::lcm(other.denominator, this->denominator);
   unsigned num = (other.numerator*(den/other.denominator) + this->numerator*(den/this->denominator));

   time result(num, den);

   return result;
}

bool time::operator==(const time& other)
{
   return (other.denominator == this->denominator)&&(other.numerator, this->numerator);
}

bool time::operator!=(const time& other)
{
   return !(*this==other);
}

//Prints the time
std::ostream& operator<<(std::ostream& out, const music::time& time)
{
	return out << time.numerator << " / " << time.denominator;
}