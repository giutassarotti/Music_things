#ifndef SCALE_HPP
#define SCALE_HPP

#include <ostream>

namespace music
{
	enum class scale
    {
        Diesis,
        Natural,
        Bemolle
    };
}

std::ostream& operator<<(std::ostream& out, const music::scale scale);

#endif