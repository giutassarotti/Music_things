#ifndef CLEF_HPP
#define CLEF_HPP

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
	};
}

#endif