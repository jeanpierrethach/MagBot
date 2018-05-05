#pragma once

#include "Common.h"
#include "InformationManager.h"
#include "Building.h"

namespace MagBot
{
	class BuildingData
	{
		std::vector<Building> _buildings;

	public:
		BuildingData();

		std::vector<Building> & getBuildings();

		void addBuilding(const Building & building);
		void removeBuilding(const Building & b);
		void removeBuildings(const std::vector<Building> & buildings);
		bool isBeingBuilt(BWAPI::UnitType unit_type);
	};

}