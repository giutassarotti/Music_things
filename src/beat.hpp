#ifndef BEAT_HPP
#define BEAT_HPP

#include <vector>
#include <memory>
#include <utility>
#include <string>

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
			void add_figure(std::unique_ptr<figure> figure);

		public:
			time time_;
			std::vector <std::pair<scale,basic_note>> scale_;
			std::string clef;
			std::vector <std::unique_ptr<figure>> figures;
			beat(time t, std::string c, std::vector <std::pair<scale,basic_note>> s);
			beat() = default;
			bool control_himself();

			void add_figure(const note& figure);
			void add_figure(const pause& figure);
	};
}

//Prints the note
std::ostream& operator<<(std::ostream& out, const music::beat& beat);

#endif