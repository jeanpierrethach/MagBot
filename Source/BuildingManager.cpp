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

	//unit->getBuildType() ? can be interesting
	//unit->isBeingConstructed too
	
	for (auto & unit : BWAPI::Broodwar->self()->getUnits())
	{
		BWAPI::UnitType unitType = unit->getType();

		if (unitType.isBuilding() && !_buildings.contains(unit))
		{
			Broodwar->sendText("Building %s confirmed at position (%d, %d)", unitType.c_str(), 
				unit->getTilePosition().x, unit->getTilePosition().y);
			_buildings.insert(unit);		
			_buildingsMap[unit] = unitType;		
		}
	}

	showDebugBuildings();
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

// TODO add build time UI for buildings