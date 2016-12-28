#include "GameCommander.h"

using namespace MagBot;

GameCommander::GameCommander()
	: _initialScout(false)
{

}


GameCommander::~GameCommander()
{

}

void GameCommander::update()
{
	ScoutManager::Instance().update();
}


void GameCommander::setScoutUnits()
{
	// if we haven't set a scout unit, do it
	if (_scoutUnits.empty() && !_initialScout)
	{
		BWAPI::Unit supplyProvider = getFirstSupplyProvider();

		// if it exists
		if (supplyProvider)
		{
			// grab the closest worker to the supply provider to send to scout
			BWAPI::Unit workerScout = getClosestWorkerToTarget(supplyProvider->getPosition());

			// if we find a worker (which we should) add it to the scout units
			if (workerScout)
			{
				ScoutManager::Instance().setWorkerScout(workerScout);
				assignUnit(workerScout, _scoutUnits);
				_initialScout = true;
			}
		}
	}
}

void GameCommander::assignUnit(BWAPI::Unit unit, BWAPI::Unitset & set)
{
	if (_scoutUnits.contains(unit)) { _scoutUnits.erase(unit); }
	else if (_combatUnits.contains(unit)) { _combatUnits.erase(unit); }

	set.insert(unit);
}

bool GameCommander::isAssigned(BWAPI::Unit unit) const
{
	return _combatUnits.contains(unit) || _scoutUnits.contains(unit);
}

BWAPI::Unit GameCommander::getClosestWorkerToTarget(BWAPI::Position target)
{
	BWAPI::Unit closestUnit = nullptr;
	double closestDist = 100000;

	for (auto & unit : _validUnits)
	{
		if (!isAssigned(unit) && unit->getType().isWorker() )//&& WorkerManager::Instance().isFree(unit))
		{
			double dist = unit->getDistance(target);
			if (!closestUnit || dist < closestDist)
			{
				closestUnit = unit;
				closestDist = dist;
			}
		}
	}

	return closestUnit;
}

BWAPI::Unit GameCommander::getFirstSupplyProvider()
{
	BWAPI::Unit supplyProvider = nullptr;

	for (auto & unit : BWAPI::Broodwar->self()->getUnits())
	{
		if (unit->getType() == BWAPI::Broodwar->self()->getRace().getSupplyProvider())
		{
			supplyProvider = unit;
			break;
		}
	}	

	return supplyProvider;
}