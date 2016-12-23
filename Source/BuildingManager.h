#pragma once

#include "Common.h"

namespace MagBot
{

	class BuildingManager
	{

		// TODO container for all protoss buildings include or exclude pylons?

		BWAPI::Unitset _buildings;
		std::map<BWAPI::Unit, BWAPI::UnitType> _buildingsMap;


	public:
		BuildingManager();
		~BuildingManager();

		void update();

		void showDebugBuildings();

		const BWAPI::Unitset getBuildings() const { return _buildings; }
		
		static BuildingManager & Instance();
	};

}