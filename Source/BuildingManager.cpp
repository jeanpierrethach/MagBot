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
	for (auto & unit : BWAPI::Broodwar->self()->getUnits())
	{
		BWAPI::UnitType unitType = unit->getType();

		if (unitType.isBuilding() && !_buildings.contains(unit))
		{
			if (Config::DebugInfo::DrawAllInfo)
			{
				Broodwar->sendText("Building %s confirmed at position (%d, %d)", unitType.c_str(),
					unit->getTilePosition().x, unit->getTilePosition().y);
			}
			_buildings.insert(unit);		
			_buildingsOwnedMap[unitType] += 1;		
		}
	}

	// TODO reposition and add config debug info option
	Broodwar->drawTextScreen(200, 80, "Buildings count: %d", getBuildings().size());
	Broodwar->drawTextScreen(200, 90, "Buildings destroyed: %d", _buildingDestroyed);

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
	for (auto & unit : getBuildings())
	{
		TilePosition tilePosition = unit->getTilePosition();
		BWAPI::UnitType unitType = unit->getType();

		Broodwar->drawBoxMap(Position(tilePosition), Position(tilePosition + unitType.tileSize()), Colors::Orange);

		Broodwar->drawCircleMap(Position(tilePosition), unitType.sightRange(), Colors::Blue);
	}
}

void BuildingManager::removeBuildingsDestroyed()
{
	for (auto & unit : getBuildings())
	{
		if (!unit->exists())
		{
			_buildings.erase(unit);
			
			_buildingDestroyed += 1;

			BWAPI::UnitType unitType = unit->getType();
			_buildingsOwnedMap[unitType] -= 1;
			_buildingsDestroyedMap[unitType] += 1;
		}
	}
}

void BuildingManager::addBuildingsUnderConstruction()
{
	for (auto & unit : getBuildings())
	{
		auto it = std::find(_buildingsUnderConstruction.begin(), _buildingsUnderConstruction.end(), unit);
		if (unit->isBeingConstructed() && it == _buildingsUnderConstruction.end())
		{
			_buildingsUnderConstruction.push_back(unit);
		}
	}
}

void BuildingManager::removeBuildingsCompleted()
{
	for (size_t i = 0; i < _buildingsUnderConstruction.size(); ++i)
	{
		BWAPI::Unit building = _buildingsUnderConstruction[i];

		auto it = std::find(_buildingsUnderConstruction.begin(), _buildingsUnderConstruction.end(), building);
		if (building->getRemainingBuildTime() <= 0)
		{
			auto index = std::distance(_buildingsUnderConstruction.begin(), it);
			_buildingsUnderConstruction.erase(_buildingsUnderConstruction.begin() + index);
			_buildingsUnderConstruction.shrink_to_fit();
		}
	}
}

void BuildingManager::showBuildTimeBuildings()
{
	for (size_t i = 0; i < _buildingsUnderConstruction.size(); ++i)
	{
		BWAPI::Unit building = _buildingsUnderConstruction[i];

		Broodwar->drawTextScreen(0, (i * 10) + 70, "%s : %d", building->getType().c_str(), building->getRemainingBuildTime());
	}

	Broodwar->drawTextScreen(0, 60, "Buildings in construction: %d", _buildingsUnderConstruction.size());
}


// TODO show at all time, not just when the specific building is destroyed
void BuildingManager::showOwnedOrDestroyedBuildings()
{
	const std::map<BWAPI::UnitType, int>::const_iterator it;
	const std::map<BWAPI::UnitType, int>::const_iterator it2;
	int count = 0;
	
	for (auto & it = _buildingsOwnedMap.begin(); it != _buildingsOwnedMap.end(); ++it)
	{
		for (auto & it2 = _buildingsDestroyedMap.begin(); it2 != _buildingsDestroyedMap.end(); ++it2)
		{
			if (it2->first == it->first)
			{
				// owned : destroyed - UnitType name
				Broodwar->drawTextScreen(200, count, "%d : %d - %s\t", it->second, it2->second, it->first.c_str());
				count += 10;
			}	
		}		
	}
	
}