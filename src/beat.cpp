#include "beat.hpp"

using music::beat;

beat::beat (time t, unsigned int p, clef c, scale s):
	time_{t}, position{p}, clef_{c}, scale_{s}
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