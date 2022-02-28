#include "beat.hpp"

using music::beat;
using music::time;

beat::beat (time t, clef c, std::vector <scale> s):
	time_{t}, clef_{c}, scale_{s}
{}

bool beat::control_himself ()
{
	music::time tot(0,0);
	
	for (auto& f: figures)
	{
		tot = tot + f->length;
	}

	if (tot != time_)
	{
		//error
		return false;
	}

	return true;
}

void beat::add_figure(std::unique_ptr<figure> figure)
{
	figures.push_back(std::move(figure));
}

void beat::add_figure(const note& figure)
{
	figures.push_back(std::make_unique<note>(figure));
}

void beat::add_figure(const pause& figure)
{
	figures.push_back(std::make_unique<pause>(figure));
}

std::ostream& operator<<(std::ostream& out, const music::beat& beat)
{
	for (auto& f: beat.figures)
	{
		out << f << '\n';
	}

	return out;
}

//controllo scala??