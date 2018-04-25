#pragma once

#include "Common.h"
#include "InformationManager.h"

namespace MagBot
{
	namespace BuildingStatus
	{
		enum { UNASSIGNED = 0, ASSIGNED = 1, UNDERCONSTRUCTION = 2 };
	}

	class Building
	{
	public:

		BWAPI::TilePosition _final_position;
		BWAPI::TilePosition _desired_position;
		BWAPI::UnitType _unit_type;
		BWAPI::Unit _building_unit;
		BWAPI::Unit _builder_unit;
		size_t _status;

		bool _under_construction;
		bool _build_command_given;

		Building()
			: _final_position(BWAPI::TilePositions::None)
			, _unit_type(BWAPI::UnitTypes::Unknown)
			, _building_unit(nullptr)
			, _builder_unit(nullptr)
			, _status(BuildingStatus::UNASSIGNED)
			, _under_construction(false)
		{
		}

		Building(BWAPI::UnitType unit_type)
			: _final_position(0, 0)
			, _unit_type(unit_type)
			, _building_unit(nullptr)
			, _builder_unit(nullptr)
			, _status(BuildingStatus::UNASSIGNED)
			, _under_construction(false)
		{
		}

		bool operator == (const Building & b)
		{
			return (b._building_unit == _building_unit) || (b._builder_unit == _builder_unit);
		}
	};

	class BuildingData : InformationManager
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