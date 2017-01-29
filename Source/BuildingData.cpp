#include "BuildingData.h"

using namespace MagBot;

BuildingData::BuildingData()
{
}

void BuildingData::removeBuilding(const Building & b)
{
	const auto & building = std::find(_buildings.begin(), _buildings.end(), b);

	if (building != _buildings.end())
	{
		_buildings.erase(building);
	}
}

std::vector<Building> & BuildingData::getBuildings()
{
	return _buildings;
}

void BuildingData::addBuilding(const Building & building)
{
	_buildings.push_back(building);
}

bool BuildingData::isBeingBuilt(BWAPI::UnitType unit_type)
{
	for (const auto & building : _buildings)
	{
		if (building._unit_type == unit_type)
		{
			return true;
		}
	}
	return false;
}

void BuildingData::removeBuildings(const std::vector<Building> & buildings)
{
	for (const auto & building : buildings)
	{
		removeBuilding(building);
	}
}