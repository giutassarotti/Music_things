#ifndef BASIC_NOTE_HPP
#define BASIC_NOTE_HPP

#include <ostream>

namespace music
{
	enum class basic_note
    {
        DO,
        RE,
        MI,
        FA,
        SOL,
        LA,
        SI
    };
}

//Prints the stupid enum class for basic_notes
std::ostream& operator<<(std::ostream& out, const music::basic_note note);

#endif