#include "BuildingManager.h"

using namespace MagBot;
using namespace BWAPI;

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

void BuildingManager::update()
{
	for (const auto & unit : BWAPI::Broodwar->self()->getUnits())
	{
		BWAPI::UnitType unit_type = unit->getType();

		if (unit_type.isBuilding() && !_buildings.contains(unit))
		{
			if (Config::DebugInfo::DrawAllInfo)
			{
				Broodwar->sendText("Building %s confirmed at position (%d, %d)", unit_type.c_str(),
					unit->getTilePosition().x, unit->getTilePosition().y);
			}
			_buildings.insert(unit);		
			_buildings_owned_map[unit_type] += 1;

			if (_buildings_destroyed_map.find(unit_type) == _buildings_destroyed_map.end())
			{
				_buildings_destroyed_map[unit_type] = 0;
			}
		}
	}

	removeBuildingsDestroyed();
	addBuildingsUnderConstruction();
	removeBuildingsCompleted();

	if (Config::DebugInfo::DrawAllInfo)
	{
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
}

void BuildingManager::showDebugBuildings()
{
	for (const auto & unit : getBuildings())
	{
		TilePosition tilePosition = unit->getTilePosition();
		BWAPI::UnitType unit_type = unit->getType();

		Broodwar->drawBoxMap(Position(tilePosition), Position(tilePosition + unit_type.tileSize()), Colors::Orange);
		Broodwar->drawCircleMap(Position(tilePosition), unit_type.sightRange(), Colors::Blue);
	}
}

void BuildingManager::removeBuildingsDestroyed()
{
	for (const auto & unit : getBuildings())
	{
		if (!unit->exists())
		{
			_buildings.erase(unit);		
			_building_destroyed += 1;

			BWAPI::UnitType unit_type = unit->getType();
			_buildings_owned_map[unit_type] -= 1;
			_buildings_destroyed_map[unit_type] += 1;
		}
	}
}

void BuildingManager::addBuildingsUnderConstruction()
{
	for (const auto & unit : getBuildings())
	{
		auto it = std::find(_buildings_under_construction.begin(), _buildings_under_construction.end(), unit);
		if (unit->isBeingConstructed() && it == _buildings_under_construction.end())
		{
			_buildings_under_construction.push_back(unit);
		}
	}
}

void BuildingManager::removeBuildingsCompleted()
{
	for (size_t i = 0; i < _buildings_under_construction.size(); ++i)
	{
		BWAPI::Unit building = _buildings_under_construction[i];

		auto it = std::find(_buildings_under_construction.begin(), _buildings_under_construction.end(), building);
		if (building->getRemainingBuildTime() <= 0)
		{
			auto index = std::distance(_buildings_under_construction.begin(), it);
			_buildings_under_construction.erase(_buildings_under_construction.begin() + index);
			_buildings_under_construction.shrink_to_fit();
		}
	}
}

void BuildingManager::showBuildTimeBuildings()
{
	for (size_t i = 0; i < _buildings_under_construction.size(); ++i)
	{
		BWAPI::Unit building = _buildings_under_construction[i];
		Broodwar->drawTextScreen(0, (i * 10) + 70, "%s : %d", building->getType().c_str(), building->getRemainingBuildTime());
	}
	Broodwar->drawTextScreen(0, 60, "Buildings in construction: %d", _buildings_under_construction.size());
}

void BuildingManager::showOwnedOrDestroyedBuildings()
{
	std::map<BWAPI::UnitType, int>::const_iterator it;
	std::map<BWAPI::UnitType, int>::const_iterator it2;
	int count = 0;

	for (it = _buildings_owned_map.begin(); it != _buildings_owned_map.end(); ++it)
	{
		for (it2 = _buildings_destroyed_map.begin(); it2 != _buildings_destroyed_map.end(); ++it2)
		{
			if (it->first == it2->first)
			{
				// owned : destroyed - UnitType name
				Broodwar->drawTextScreen(200, count, "%d : %d - %s\t", it->second, it2->second, it->first.c_str());
				count += 10;
			}
		}
	}	
	Broodwar->drawTextScreen(200, 80, "Buildings count: %d", getBuildings().size());
	Broodwar->drawTextScreen(200, 90, "Buildings destroyed: %d", _building_destroyed);
}