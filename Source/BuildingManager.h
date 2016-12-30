#pragma once

#include "Common.h"
#include "BuildingData.h"
#include "WorkerManager.h"

namespace MagBot
{
	class BuildingManager
	{
		std::vector<Building> _buildings;

		int _reservedMinerals; // minerals reserved for planned buildings
		int  _reservedGas; // gas reserved for planned buildings

		// TODO better way for this
		BWAPI::Unitset _all_buildings;
		std::map<BWAPI::UnitType, int> _buildings_owned_map;
		std::map<BWAPI::UnitType, int> _buildings_destroyed_map;
		int _building_destroyed = 0;
		void removeBuildingsDestroyed();
		void showAllBuildings();
		void showDebugBuildings();
		void showBuildTimeBuildings();
		void showOwnedOrDestroyedBuildings();

		void removeBuildings(const std::vector<Building> & to_remove_buildings);

		void validateWorkersAndBuildings(); // STEP 1
		void assignWorkersToUnassignedBuildings(); // STEP 2
		void constructAssignedBuildings(); // STEP 3
		void checkForStartedConstruction();	// STEP 4
		void checkForCompletedBuildings(); // STEP 5

	public:
		BuildingManager();
		~BuildingManager();

		void update();
		void addBuildingTask(BWAPI::UnitType type);
		bool isBeingBuilt(BWAPI::UnitType unit_type);	

		int getReservedMinerals();
		int getReservedGas();

		static BuildingManager & Instance();
	};
}