#ifndef NOTE_HPP
#define NOTE_HPP

#include <ostream>

#include "time.hpp"
#include "scale.hpp"

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

    struct figure
    {
        time length;				//lunghezza

        figure(time l);
    };

	struct note: figure
	{
		basic_note basic_note_;	    // es DO
		//short which_basic_note;	// es 2, quindi do2 totale

        scale scale_;

		note(basic_note n, time l, scale s);
	};

    struct pause: figure
    {
        pause(time l);
    };
}

//Prints the stupid enum class for basic_notes
std::ostream& operator<<(std::ostream& out, const music::basic_note note);

//Prints the note
std::ostream& operator<<(std::ostream& out, const music::note& note);

//Prints the pause
std::ostream& operator<<(std::ostream& out, const music::pause& pause);

#endif