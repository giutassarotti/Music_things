#ifndef CLEF_HPP
#define CLEF_HPP

#include "time.hpp"
#include "scale.hpp"
#include "note.hpp"

namespace music
{
    // enum class basic_clef
    // {
    //     violin,
    //     soprano,
    //     mezzo_soprano,
    //     contralto,
    //     tenor,
    //     baritone,
    //     bass
    // };

    class clef
	{
        public:
		    //basic_clef basic_clef_;	// es violin

		    //clef(basic_clef basic_clef_);
            clef() = delete;
            
            static note violin(short position, time length, scale scal);
            //note bass();

            static basic_note violin_alt(short position);
	};
}

#endif