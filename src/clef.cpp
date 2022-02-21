#include "clef.hpp"
#include "note.hpp"

#include <string>
#include <vector>

using music::clef;

clef::clef (basic_clef c):
    basic_clef_{c}
{};

note clef::violin(basic_clef cleff, short position, time length, scale scal)
{
    //From the lowest line to the uppest line
    std::vector <std::string> notes = {"MI", "FA", "SOL", "LA", "SI", "DO", "RE", "MI", "FA"};

    note N;

    return N;
}