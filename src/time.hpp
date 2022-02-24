#ifndef TIME_HPP
#define TIME_HPP

namespace music
{
    struct time
    {
        unsigned numerator;
        unsigned denominator;

        time(unsigned n, unsigned d);

        time() = default;

        time operator+(const time& other);
        bool operator==(const time& other);
        bool operator!=(const time& other);
    };
}

#endif