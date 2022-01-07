#ifndef BEAT_HPP
#define BEAT_HPP

#include <vector>
#include "note.hpp"
#include "time.hpp"
#include "clef.hpp"
#include "scale.hpp"

namespace music
{
	class beat
	{
		private:
			time time_;
			unsigned int position;
			clef clef_;
			std::vector <scale> scale_;
			std::vector <note> notes;

		public:
			beat(time t, unsigned int p, clef c, scale s);
			bool control_himself();
	};
}

#endif