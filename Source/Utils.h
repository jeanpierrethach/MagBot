#pragma once

#include "Common.h"

namespace MagBot
{

	class Utils
	{
	public:
		Utils();
		~Utils();

		double radToDeg(double angle);
		double getAngleDeg(BWAPI::Position p1, BWAPI::Position p2);
		double getAngleRad(BWAPI::Position p1, BWAPI::Position p2);
		int getDistance(BWAPI::Position p1, BWAPI::Position p2);
		void average(BWAPI::Position & pos, int size);
		void normalize(BWAPI::Position & pos);

	};

}

