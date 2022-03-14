#include "clef.hpp"
#include "note.hpp"

#include <vector>

using music::note;
using music::basic_note;
using music::clef;

// clef::clef (basic_clef c):
//     basic_clef_{c}
// {};

note clef::violin(short position, time length, scale scal)
{
    //From the lowest line to the uppest line
    std::vector <basic_note> notes = 
    {
        basic_note::SOL,
        basic_note::FA,
        basic_note::MI,
        basic_note::RE,
        basic_note::DO,
        basic_note::SI,
        basic_note::LA,
        basic_note::SOL,
        basic_note::FA, 
        basic_note::MI,
        basic_note::RE
    };

    //TODO note fori dal balcone
    //TODO do1 do2 do-tanti

    note nota(notes[position+1], length, scal);

    return nota;
}

basic_note clef::violin_alt(short position)
{
    //From the lowest line to the uppest line
    std::vector <basic_note> notes = 
    {
        basic_note::SOL,
        basic_note::FA,
        basic_note::MI,
        basic_note::RE,
        basic_note::DO,
        basic_note::SI,
        basic_note::LA,
        basic_note::SOL,
        basic_note::FA, 
        basic_note::MI,
        basic_note::RE
    };

    return notes[position+1];
}