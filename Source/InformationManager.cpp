#include "InformationManager.h"

using namespace MagBot;

InformationManager::InformationManager()
{
}


InformationManager::~InformationManager()
{
}

double InformationManager::calculateMineralRate()
{
	if (BWAPI::Broodwar->getFrameCount() != 0)
	{
		int nb_min = BWAPI::Broodwar->self()->gatheredMinerals();
		int nb_frames = BWAPI::Broodwar->getFrameCount();
		_minerals_rate = (double) nb_min / nb_frames;
		return _minerals_rate;
	}
	return 0;
}

double InformationManager::calculateGasRate()
{
	if (BWAPI::Broodwar->getFrameCount() != 0)
	{
		int nb_gas = BWAPI::Broodwar->self()->gatheredGas();
		int nb_frames = BWAPI::Broodwar->getFrameCount();
		_gas_rate = (double) nb_gas / nb_frames;
		return _gas_rate;
	}
	return 0;
}

double InformationManager::calculateMineralRatePerSec()
{
	if (BWAPI::Broodwar->getFrameCount() != 0)
	{
		int nb_min = BWAPI::Broodwar->self()->minerals();
		double nb_frames = BWAPI::Broodwar->getFrameCount() / 24;
		return (double) nb_min / nb_frames;
	}
	return 0;
}

double InformationManager::calculateGasRatePerSec()
{
	if (BWAPI::Broodwar->getFrameCount() != 0)
	{
		int nb_gas = BWAPI::Broodwar->self()->gas();
		double nb_frames = BWAPI::Broodwar->getFrameCount() / 24;
		return (double) nb_gas / nb_frames;
	}
	return 0;
}

void InformationManager::setEnemyStartingBaseLocation(const BWAPI::Position pos)
{
	_enemy_base_location = pos;
}

void InformationManager::setStartingBaseLocation(const BWAPI::Position pos)
{
	_starting_base_location = pos;
}