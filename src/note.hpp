#ifndef NOTE_HPP
#define NOTE_HPP

#include "time.hpp"
#include "clef.hpp"
#include "scale.hpp"

#include <string>

namespace music
{
	// enum class basic_note
    // {
    //     DO,
    //     RE,
    //     MI,
    //     FA,
    //     SOL,
    //     LA,
    //     SI
    // };

	struct note
	{
		std::string basic_note_;	// es DO
		short which_basic_note;		// es 2, quindi do2 totale

        time length;				//lunghezza
        scale scale_;

		note(clef c, short p, time l, scale s);
	};
}

#endif