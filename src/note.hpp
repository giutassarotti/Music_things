#ifndef NOTE_HPP
#define NOTE_HPP

#include <ostream>

#include "time.hpp"
#include "scale.hpp"
#include "basic_note.hpp"

using music::basic_note;

namespace music
{
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

//Prints the note
std::ostream& operator<<(std::ostream& out, const music::note& note);

//Prints the pause
std::ostream& operator<<(std::ostream& out, const music::pause& pause);

#endif