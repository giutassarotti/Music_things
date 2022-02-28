#include "note.hpp"


using music::figure;
using music::pause;
using music::note;

figure::figure(time l):
	length(l)
{}

pause::pause(time l):
	figure(l)
{}

note::note (basic_note n, time l, scale s):
	figure(l),
	basic_note_{n}, scale_{s}
{}

std::ostream& operator<<(std::ostream& out, const music::note& note)
{
	//return out << note.basic_note_ << ' ' << note.scale_ << ' ' << note.length;
	return out << note.basic_note_ << ' ' << note.length;
}

std::ostream& operator<<(std::ostream& out, const music::pause& pause)
{
	return out << pause.length;
}