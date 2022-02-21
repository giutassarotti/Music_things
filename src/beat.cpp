#include "beat.hpp"

using music::beat;

beat::beat (time t, clef c, scale s):
	time_{t}, clef_{c}, scale_{s}
{}

bool beat::control_himself ()
{
	time tot;
	for (auto& n: notes)
	{
		tot = tot + n.length;
	}

	if (tot != time_)
	{
		//error
		return false;
	}

	return true;
}

//controllo scala??