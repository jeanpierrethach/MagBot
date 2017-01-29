#pragma once

#include "Common.h"

namespace MagBot
{

	class Pylon
	{
	public:
		BWAPI::Unit _unit;
		BWAPI::UnitType _unit_type;
		int _hp;
		int _shield;
		int _armor;
		BWAPI::TilePosition _pos;

		Pylon()
			: _unit(nullptr)
			, _unit_type(BWAPI::UnitTypes::Protoss_Pylon)
			, _hp(0)
			, _shield(0)
			, _armor(0)
			, _pos(BWAPI::TilePositions::None)
		{
		}
		// TODO add and update values of pylons existing
		Pylon(BWAPI::Unit unit)
			: _unit(unit)
			, _unit_type(BWAPI::UnitTypes::Protoss_Pylon)
			, _hp(0)
			, _shield(0)
			, _armor(0)
			, _pos(BWAPI::TilePositions::None)
		{
		}

		bool operator == (const Pylon & p)
		{
			return (p._unit == _unit);
		}
	};

	class PylonData
	{
		std::vector<Pylon> _pylons;

		// TODO may remove this container?
		std::map<BWAPI::Unit, Pylon> _pylon_map;

	public:
		PylonData();
		~PylonData();

		void addPylon(const Pylon & pylon);
		void removePylon(const Pylon & pylon);
		//bool exists(BWAPI::Unit unit);

		BWAPI::Position getPylonPosition(const Pylon & pylon);
		BWAPI::TilePosition getPylonTilePosition(const Pylon & pylon);

		//const BWAPI::Unitset getPylons() const { return _pylons; }
	};

}