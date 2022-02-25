#ifndef TIME_HPP
#define TIME_HPP

#include <ostream>

namespace music
{
    struct time
    {
        unsigned numerator;
        unsigned denominator;

        time(unsigned n, unsigned d);

        time operator+(const time& other);
        bool operator==(const time& other);
        bool operator!=(const time& other);

    };
}

//Prints the time
std::ostream& operator<<(std::ostream& out, const music::time& time);

#endif