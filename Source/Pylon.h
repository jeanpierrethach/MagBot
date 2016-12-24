#pragma once

#include "Common.h"

namespace MagBot
{

	struct PylonData
	{
		//int hp;
		//int shield;
		//int armor;
		//TilePosition;
	};

	class Pylon
	{
		BWAPI::Unitset _pylons;
		std::map<BWAPI::Unit, PylonData> _pylonMap;

	public:
		Pylon();
		~Pylon();

		//void addPylon(BWAPI::Unit unit);
		//void removePylon(BWAPI::Unit unit);
		//bool exists(BWAPI::Unit unit);

		//Position getPylonPosition(BWAPI::Unit unit);
		//TilePosition getPylonTilePosition(BWAPI::Unit unit);

		//const BWAPI::Unitset getPylons() const { return _pylons; }
	};

}