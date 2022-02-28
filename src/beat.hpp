#ifndef BEAT_HPP
#define BEAT_HPP

#include <vector>
#include <memory>

#include "note.hpp"
#include "time.hpp"
#include "clef.hpp"
#include "scale.hpp"
#include "basic_note.hpp"

namespace music
{
	struct beat
	{
		private:
			time time_;
			clef clef_;
			std::vector <scale> scale_;
			
			void add_figure(std::unique_ptr<figure> figure);

		public:
			std::vector <std::unique_ptr<figure>> figures;
			beat(time t, clef c, std::vector <scale> s);
			bool control_himself();

			void add_figure(const note& figure);
			void add_figure(const pause& figure);
	};
}

//Prints the note
std::ostream& operator<<(std::ostream& out, const music::beat& beat);

#endif