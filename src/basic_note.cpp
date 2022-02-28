#include "basic_note.hpp"

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