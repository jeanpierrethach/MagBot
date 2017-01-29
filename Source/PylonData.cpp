#include "PylonData.h"


using namespace MagBot;

PylonData::PylonData()
{
}


PylonData::~PylonData()
{
}

void PylonData::addPylon(const Pylon & pylon)
{
	_pylons.push_back(pylon);
}

void PylonData::removePylon(const Pylon & p)
{
	const auto & pylon = std::find(_pylons.begin(), _pylons.end(), p);

	if (pylon != _pylons.end())
	{
		_pylons.erase(pylon);
	}
}

BWAPI::Position PylonData::getPylonPosition(const Pylon & pylon)
{
	return BWAPI::Position(pylon._pos);
}

BWAPI::TilePosition PylonData::getPylonTilePosition(const Pylon & pylon)
{
	return pylon._pos;
}

// TODO calculate how many buildings this pylon is powering

// TODO getTilePosition that can power the most buildings completed

// TODO getTilePosition that will not power any existing buildings

// TODO getTilePosition next to this pylon that is available