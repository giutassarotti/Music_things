#include "scale.hpp"

std::ostream& operator<<(std::ostream& out, const music::scale scale)
{
	switch (scale) 
	{
		case music::scale::Diesis: 
			return out << "Diesis";
        case music::scale::Natural:
			return out << "Natural";
        case music::scale::Bemolle:
			return out << "Bemolle";
	}

	return out;
}