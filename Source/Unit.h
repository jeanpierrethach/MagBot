#pragma once

#include "Common.h"

namespace MagBot
{
	// TODO generalize class so it can describe units as
	// either buildings or bwapi::unit (any unit)
	class Unit
	{
	public:
		BWAPI::Unit unit;

		BWAPI::Position pos;
		bool has_position;
		bool has_sing_charge;
		int range;
		BWAPI::Position target_pos;

		Unit()
		{
		}

		Unit(BWAPI::Unit unit)
			: pos(BWAPI::Positions::None)
			, has_position(false)
			, has_sing_charge(false)
			, unit(unit)
			, target_pos(BWAPI::Positions::None)
		{
		}

		void resetPosition();

		bool operator == (const Unit & p) const
		{
			return (p.pos == pos);
		}
	};

}