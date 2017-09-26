#include "BuildingManager.h"

using namespace MagBot;

BuildingManager::BuildingManager()
{
}


BuildingManager::~BuildingManager()
{
}

BuildingManager & BuildingManager::Instance()
{
	static BuildingManager instance;
	return instance;
}

void BuildingManager::onUnitDestroy(BWAPI::Unit unit)
{
	if (!unit)
		return;

	if (unit->getType().isBuilding() && unit->getPlayer() == BWAPI::Broodwar->self())
	{
		removeBuildingDestroyed(unit);
	}
}

void BuildingManager::update()
{
	validateWorkersAndBuildings();          // check to see if assigned workers have died en route or while constructing
	assignWorkersToUnassignedBuildings();   // assign workers to the unassigned buildings and label them 'planned'    
	constructAssignedBuildings();           // for each planned building, if the worker isn't constructing, send the command    
	checkForStartedConstruction();          // check to see if any buildings have started construction and update data structures        
	checkForCompletedBuildings();           // check to see if any buildings have completed and update data structures

	if (Config::DebugInfo::DrawAllInfo)
	{
		if (Config::DebugInfo::DrawBuildingsCount)
		{
			showAllBuildings();
		}
		if (Config::DebugInfo::DrawBuildingTilesInfo)
		{
			showDebugBuildings();
		}
		if (Config::DebugInfo::DrawBuildingTimeInConstruction)
		{
			showBuildTimeBuildings();
		}
		if (Config::DebugInfo::DrawBuildingsOwnedOrDestroyed)
		{
			showOwnedOrDestroyedBuildings();
		}
	}

	BWAPI::Broodwar->drawTextScreen(360, 130, "Mineral Rate %f", _inf_manager.calculateMineralRate());
	BWAPI::Broodwar->drawTextScreen(360, 140, "Gas Rate %f", _inf_manager.calculateGasRate());
	BWAPI::Broodwar->drawTextScreen(360, 150, "Minerate Rate/sec %d", BWAPI::Broodwar->getLatency());
	BWAPI::Broodwar->drawTextScreen(360, 160, "Gas Rate/sec");
}

void BuildingManager::showAllBuildings()
{
	for (const auto & unit : BWAPI::Broodwar->self()->getUnits())
	{
		BWAPI::UnitType unit_type = unit->getType();

		if (unit_type.isBuilding() && !_all_buildings.contains(unit))
		{
			if (Config::DebugInfo::DrawAllInfo)
			{
				BWAPI::Broodwar->sendText("Building %s confirmed at position (%d, %d)", unit_type.c_str(),
					unit->getTilePosition().x, unit->getTilePosition().y);
			}
			_all_buildings.insert(unit);
			++_buildings_owned_map[unit_type];

			if (_buildings_destroyed_map.find(unit_type) == _buildings_destroyed_map.end())
			{
				_buildings_destroyed_map[unit_type] = 0;
			}
		}
	}
}

void BuildingManager::showDebugBuildings()
{
	for (const auto & building : _all_buildings)
	{
		BWAPI::TilePosition tilePosition = building->getTilePosition();
		BWAPI::UnitType unit_type = building->getType();

		BWAPI::Broodwar->drawBoxMap(BWAPI::Position(tilePosition), BWAPI::Position(tilePosition + unit_type.tileSize()), BWAPI::Colors::Orange);
		BWAPI::Broodwar->drawCircleMap(BWAPI::Position(tilePosition), unit_type.sightRange(), BWAPI::Colors::Blue);
	}
}

void BuildingManager::removeBuildingDestroyed(BWAPI::Unit unit)
{
	if (!unit)
		return;

	_all_buildings.erase(unit);

	BWAPI::UnitType unit_type = unit->getType();
	--_buildings_owned_map[unit_type];
	++_buildings_destroyed_map[unit_type];
	++_building_destroyed;
}

void BuildingManager::showBuildTimeBuildings()
{
	uint8_t building_count {0};

	for (const auto & building : _buildings.getBuildings())
	{
		// TODO FIX display when, example : 3 pylons are being queued but 2 only built since one of them couldn't find a location 
		if (building._status == BuildingStatus::UNDERCONSTRUCTION)
		{
			BWAPI::Broodwar->drawTextScreen(0, (building_count * 10) + 70, "%s : %d",
				building._unit_type.c_str(), building._building_unit->getRemainingBuildTime());

			++building_count;
		}
	}
	BWAPI::Broodwar->drawTextScreen(0, 60, "Buildings in construction: %d", building_count);
}

void BuildingManager::showOwnedOrDestroyedBuildings()
{
	std::map<BWAPI::UnitType, uint8_t>::const_iterator it;
	std::map<BWAPI::UnitType, uint8_t>::const_iterator it2;
	uint8_t count {0};

	for (it = _buildings_owned_map.begin(); it != _buildings_owned_map.end(); ++it)
	{
		for (it2 = _buildings_destroyed_map.begin(); it2 != _buildings_destroyed_map.end(); ++it2)
		{
			if (it->first == it2->first)
			{
				// owned : destroyed - UnitType name
				BWAPI::Broodwar->drawTextScreen(175, count, "%d : %d - %s\t", it->second, it2->second, it->first.c_str());
				count += 10;
			}
		}
	}	
	BWAPI::Broodwar->drawTextScreen(480, 80, "Buildings count: %d", _all_buildings.size());
	BWAPI::Broodwar->drawTextScreen(480, 90, "Buildings destroyed: %d", _building_destroyed);
}

// STEP 1:
void BuildingManager::validateWorkersAndBuildings()
{
	std::vector<Building> to_remove_buildings;

	for (const auto & b : _buildings.getBuildings())
	{
		if (b._status != BuildingStatus::UNDERCONSTRUCTION)
		{
			continue;
		}
		if (b._building_unit == nullptr || !b._building_unit->getType().isBuilding() || b._building_unit->getHitPoints() <= 0)
		{
			to_remove_buildings.push_back(b);
		}
	}

	_buildings.removeBuildings(to_remove_buildings);
}

// STEP 2:
void BuildingManager::assignWorkersToUnassignedBuildings()
{
	for (Building & b : _buildings.getBuildings())
	{
		if (b._status != BuildingStatus::UNASSIGNED)
		{
			continue;
		}

		BWAPI::Unit worker_to_assign = WorkerManager::Instance().getBuilder(b);

		if (worker_to_assign)
		{
			BWAPI::TilePosition pos = worker_to_assign->getTilePosition();
			b._builder_unit = worker_to_assign;

			// TODO fix, since getbuildlocation happens to be invalid (checking with basic AI), it will set the valid 
			// worker_to_assign to BUILD even before in the WorkerManager, so the test_location has to work.
			// Solution move getBuildLocation logic before setting BUILD to worker OR when worker is BUILDER and mining
			// change its task?
			BWAPI::TilePosition test_location = BWAPI::Broodwar->getBuildLocation(b._unit_type, pos);
			
			if (Config::DebugInfo::DrawAllInfo)
			{
				BWAPI::Broodwar->registerEvent([test_location, b](BWAPI::Game*)
				{ BWAPI::Broodwar->drawBoxMap(BWAPI::Position(test_location),
				BWAPI::Position(test_location + b._unit_type.tileSize()), BWAPI::Colors::Green); },   // action
				nullptr,    // condition
				42);  // frames to run
			}

			if (!test_location.isValid())
			{
				WorkerManager::Instance().setWorkerFree(worker_to_assign);
				b._builder_unit = nullptr;
				continue;
			}
			b._final_position = test_location;
			b._status = BuildingStatus::ASSIGNED;
		}
	}
}

// STEP 3:
void BuildingManager::constructAssignedBuildings()
{
	for (auto & b : _buildings.getBuildings())
	{
		if (b._status != BuildingStatus::ASSIGNED)
		{
			continue;
		}

		if (!b._builder_unit->isConstructing())
		{
		    if (b._build_command_given)
			{
				WorkerManager::Instance().setWorkerFree(b._builder_unit);

				b._builder_unit = nullptr;
				b._build_command_given = false;
				b._status = BuildingStatus::UNASSIGNED;
			}
			else
			{
				b._builder_unit->build(b._unit_type, b._final_position);
				b._build_command_given = true;
			}
		}
	}
}

// STEP 4:
void BuildingManager::checkForStartedConstruction()
{
	for (auto & building_started : BWAPI::Broodwar->self()->getUnits())
	{
		if (!building_started->getType().isBuilding() || !building_started->isBeingConstructed())
		{
			continue;
		}

		for (auto & b : _buildings.getBuildings())
		{
			if (b._status != BuildingStatus::ASSIGNED)
			{
				continue;
			}

			if (b._final_position == building_started->getTilePosition())
			{
				_reservedMinerals -= building_started->getType().mineralPrice();
				_reservedGas -= building_started->getType().gasPrice();

				b._under_construction = true;
				b._building_unit = building_started;

				WorkerManager::Instance().setWorkerFree(b._builder_unit);
				b._builder_unit = nullptr;

				b._status = BuildingStatus::UNDERCONSTRUCTION;
				break;
			}
		}
	}
}

// STEP 5:
void BuildingManager::checkForCompletedBuildings()
{
	std::vector<Building> to_remove_buildings;

	for (const auto & building : _buildings.getBuildings())
	{
		if (building._status != BuildingStatus::UNDERCONSTRUCTION)
		{
			continue;
		}
		if (building._building_unit->isCompleted())
		{
			to_remove_buildings.push_back(building);
		}
	}
	_buildings.removeBuildings(to_remove_buildings);
}

void BuildingManager::addBuildingTask(BWAPI::UnitType unit_type)
{
	_reservedMinerals += unit_type.mineralPrice();
	_reservedGas += unit_type.gasPrice();

	Building building(unit_type);
	building._status = BuildingStatus::UNASSIGNED;

	_buildings.addBuilding(building);
}

int BuildingManager::getReservedMinerals()
{
	return _reservedMinerals;
}

int BuildingManager::getReservedGas()
{
	return _reservedGas;
}