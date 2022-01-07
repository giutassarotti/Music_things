#include "time.hpp"

using music::time;

time time::operator+(const time& other)
{
    time result;

    result.denominator = lcm(other.denominator, this->denominator);
    result.nominator = (other.nominator*(result.denominator/other.denominator) + this->nominator*(result.denominator/this->denominator));

    return result;
}

bool time::operator==(const time& other)
{
   return (other.denominator == this->denominator)&&(other.nominator, this->nominator);
}

bool time::operator==(const time& other)
{
   return !*this==other;
}