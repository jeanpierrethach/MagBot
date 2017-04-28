#pragma once

#include "Common.h"
#include "BuildingData.h"
#include "WorkerManager.h"
#include <stdint.h>

namespace MagBot
{
	class BuildingManager
	{
		//std::vector<Building> _buildings;
		BuildingData _buildings;

		int _reservedMinerals; // minerals reserved for planned buildings
		int _reservedGas; // gas reserved for planned buildings

		// TODO better way for this
		BWAPI::Unitset _all_buildings;
		std::map<BWAPI::UnitType, uint8_t> _buildings_owned_map;
		std::map<BWAPI::UnitType, uint8_t> _buildings_destroyed_map;
		uint16_t _building_destroyed;

		void removeBuildingDestroyed(BWAPI::Unit unit);
		void showAllBuildings();
		void showDebugBuildings();
		void showBuildTimeBuildings();
		void showOwnedOrDestroyedBuildings();

		//void removeBuildings(const std::vector<Building> & to_remove_buildings);

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

		int getReservedMinerals();
		int getReservedGas();

		void onUnitDestroy(BWAPI::Unit unit);

		static BuildingManager & Instance();
	};
}