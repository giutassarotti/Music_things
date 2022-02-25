#ifndef BEAT_HPP
#define BEAT_HPP

#include <vector>
#include <memory>

#include "note.hpp"
#include "time.hpp"
#include "clef.hpp"
#include "scale.hpp"

namespace music
{
	struct beat
	{
		private:
			time time_;
			clef clef_;
			std::vector <scale> scale_;
			std::vector <std::unique_ptr<figure>> figures;

			void add_figure(std::unique_ptr<figure> figure);

		public:
			beat(time t, clef c, scale s);
			bool control_himself();

			void add_figure(const note& figure);
			void add_figure(const pause& figure);
	};
}

#endif