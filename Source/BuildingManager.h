#pragma once

#include "Common.h"

namespace MagBot
{

	class BuildingManager
	{

		// TODO container for all protoss buildings include or exclude pylons?
		// TODO update on each frame the count of minerals and gas owned -> make sure when something is in queue, other orders aren't
		// interfering

		BWAPI::Unitset _buildings;
		std::vector<BWAPI::Unit> _buildingsUnderConstruction;
		std::map<BWAPI::Unit, BWAPI::UnitType> _buildingsMap;


	public:
		BuildingManager();
		~BuildingManager();

		void update();

		void addBuildingsUnderConstruction();
		void removeBuildingsCompleted();

		void showDebugBuildings();
		void showBuildTimeBuildings();

		const BWAPI::Unitset getBuildings() const { return _buildings; }
		
		static BuildingManager & Instance();
	};

}