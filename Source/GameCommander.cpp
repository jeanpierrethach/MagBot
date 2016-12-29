#include "GameCommander.h"

using namespace MagBot;

GameCommander::GameCommander()
	: _initial_scout(false)
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
	if (_scout_units.empty() && !_initial_scout)
	{
		BWAPI::Unit supply_provider = getFirstSupplyProvider();

		// if it exists
		if (supply_provider)
		{
			// grab the closest worker to the supply provider to send to scout
			BWAPI::Unit worker_scout = getClosestWorkerToTarget(supply_provider->getPosition());

			// if we find a worker (which we should) add it to the scout units
			if (worker_scout)
			{
				ScoutManager::Instance().setWorkerScout(worker_scout);
				assignUnit(worker_scout, _scout_units);
				_initial_scout = true;
			}
		}
	}
}

void GameCommander::assignUnit(BWAPI::Unit unit, BWAPI::Unitset & set)
{
	if (_scout_units.contains(unit)) { _scout_units.erase(unit); }
	else if (_combat_units.contains(unit)) { _combat_units.erase(unit); }

	set.insert(unit);
}

bool GameCommander::isAssigned(BWAPI::Unit unit) const
{
	return _combat_units.contains(unit) || _scout_units.contains(unit);
}

BWAPI::Unit GameCommander::getClosestWorkerToTarget(BWAPI::Position target)
{
	BWAPI::Unit closest_unit = nullptr;
	double closest_dist = 100000;

	for (const auto & unit : _valid_units)
	{
		if (!isAssigned(unit) && unit->getType().isWorker() )//&& WorkerManager::Instance().isFree(unit))
		{
			double dist = unit->getDistance(target);
			if (!closest_unit || dist < closest_dist)
			{
				closest_unit = unit;
				closest_dist = dist;
			}
		}
	}
	return closest_unit;
}

BWAPI::Unit GameCommander::getFirstSupplyProvider()
{
	BWAPI::Unit supply_provider = nullptr;

	for (const auto & unit : BWAPI::Broodwar->self()->getUnits())
	{
		if (unit->getType() == BWAPI::Broodwar->self()->getRace().getSupplyProvider())
		{
			supply_provider = unit;
			break;
		}
	}	
	return supply_provider;
}