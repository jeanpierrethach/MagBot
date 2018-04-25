#pragma once

#include "Common.h"
#include "BuildingData.h"
#include "WorkerManager.h"
#include "InformationManager.h"
#include <stdint.h>

namespace MagBot
{
	class BuildingManager
	{
		BuildingData _buildings;
		InformationManager _inf_manager;

		int _reservedMinerals; // minerals reserved for planned buildings
		int _reservedGas; // gas reserved for planned buildings

		BWAPI::Unitset _all_buildings;
		std::map<BWAPI::UnitType, uint8_t> _buildings_owned_map;
		std::map<BWAPI::UnitType, uint8_t> _buildings_destroyed_map;
		uint16_t _building_destroyed;

		void removeBuildingDestroyed(BWAPI::Unit unit);
		void showAllBuildings();
		void showDebugBuildings();
		void showBuildTimeBuildings();
		void showOwnedOrDestroyedBuildings();

		void validateWorkersAndBuildings();
		void assignWorkersToUnassignedBuildings();
		void constructAssignedBuildings();
		void checkForStartedConstruction();
		void checkForCompletedBuildings();

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