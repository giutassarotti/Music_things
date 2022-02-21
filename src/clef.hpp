#ifndef CLEF_HPP
#define CLEF_HPP

namespace music
{
    enum class basic_clef
    {
        violin,
        soprano,
        mezzo_soprano,
        contralto,
        tenor,
        baritone,
        bass
    };

    struct clef
	{
		basic_clef basic_clef_;	// es violin

		clef(basic_clef basic_clef_);
        note violin();
        //note bass();
	};
}

#endif