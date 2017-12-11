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

// TODO: require testing
BWAPI::TilePosition BuildingData::findFirstAvailablePosition(const BWAPI::TilePosition tileposition)
{
	BWAPI::TilePosition base_tileposition = BWAPI::TilePosition(InformationManager::getStartingBaseLocation());
	std::vector<BWAPI::TilePosition> region;

	for (int i = 0; i < 300; i + 5)
	{
		for (int j = 0; i < 300; j + 5)
		{
			BWAPI::TilePosition tile = BWAPI::TilePosition(i, j);
			region.push_back(BWAPI::TilePosition(base_tileposition.x + tile.x, base_tileposition.y + tile.y));
			region.push_back(BWAPI::TilePosition(base_tileposition.x - tile.x, base_tileposition.y + tile.y));
			region.push_back(BWAPI::TilePosition(base_tileposition.x - tile.x, base_tileposition.y - tile.y));
			region.push_back(BWAPI::TilePosition(base_tileposition.x + tile.x, base_tileposition.y - tile.y));
		}
	}
	
	for (const auto & reg : region)
	{
		if (!reg.isValid())
			continue;
		if (BWAPI::Broodwar->isExplored(reg) && BWAPI::Broodwar->isBuildable(reg))
		{
			return reg;
		}
	}

}