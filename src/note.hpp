#ifndef NOTE_HPP
#define NOTE_HPP

#include "time.hpp"
#include "clef.hpp"
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

	struct note
	{
		time length;					//lunghezza
		basic_note basic_note_;			// es DO
		short which_basic_note;			// es 2, quindi do2 totale
        scale scale_;

		note(clef c, short p, time l, scale s);
	};
}

#endif