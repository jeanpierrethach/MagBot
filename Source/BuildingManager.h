#pragma once

#include "Common.h"

namespace MagBot
{

	class BuildingManager
	{

		// TODO container for all protoss buildings include or exclude pylons?
		std::map<BWAPI::Unit, BWAPI::UnitType> _buildings;


	public:
		BuildingManager();
		~BuildingManager();
	};

}