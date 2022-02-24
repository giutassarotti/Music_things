#include "note.hpp"

using music::note;

using std::string;

note::note (basic_note n, time l, scale s):
	basic_note_{n}, length{l}, scale_{s}
{}