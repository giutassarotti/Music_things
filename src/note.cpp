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

std::ostream& operator<<(std::ostream& out, const music::basic_note note)
{
	switch (note)
	{
		case music::basic_note::DO:
			return out << "DO"; 
        case music::basic_note::RE:
			return out << "RE";
        case music::basic_note::MI:
			return out << "MI";
        case music::basic_note::FA:
			return out << "FA";
        case music::basic_note::SOL:
			return out << "SOL";
        case music::basic_note::LA:
			return out << "LA";
        case music::basic_note::SI:
			return out << "SI";
	}

	return out;
}

std::ostream& operator<<(std::ostream& out, const music::note& note)
{
	return out << note.basic_note_ << ' ' << note.scale_ << ' ' << note.length;
}

std::ostream& operator<<(std::ostream& out, const music::pause& pause)
{
	return out << pause.length;
}