#pragma once

#include "Common.h"

namespace MagBot
{

	class Unit
	{
	private:
		BWAPI::Position _pos;
		bool _is_position_set;

	public:
		Unit()
			: _pos(BWAPI::Positions::None)
			, _is_position_set(false)
		{
		}
		Unit(BWAPI::Position pos)
			: _pos(pos)
		{
		}

		void setPosition(BWAPI::Position pos, bool b);
		BWAPI::Position getPosition();

		bool isPositionSet();
		void resetPosition();

		bool operator == (const Unit & p) const
		{
			return (p._pos == _pos);
		}
	};

}