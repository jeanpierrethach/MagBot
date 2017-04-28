#include "GameCommander.h"

using namespace MagBot;

GameCommander::GameCommander()
	: _initial_scout(false)
	, _active_scout(false)
{
}


GameCommander::~GameCommander()
{
}

void GameCommander::update()
{
	setScoutUnits();

	ScoutManager::Instance().update();

	for (const auto & unit : _scout_units)
	{
		if (!unit->exists())
		{
			_scout_units.erase(unit);
		}
	}

	if (_scout_units.empty())
		_active_scout = false;

	for (auto & scout : _scout_units)
	{
		BWAPI::Broodwar->drawCircleMap(scout->getPosition(), 20, BWAPI::Colors::Purple, false);
	}

	BWAPI::Broodwar->drawTextScreen(0, 50, "Scout : %d", _active_scout);
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
				_active_scout = true;
			}
		}
	}
}

void GameCommander::assignUnit(BWAPI::Unit unit, BWAPI::Unitset & set)
{
	if (!_scout_units.contains(unit)) 
	{ 
		set.insert(unit); 
		//_scout_units.erase(unit);
	}
	/*else if (_combat_units.contains(unit)) 
	{ 
		_combat_units.erase(unit); 
	}*/
	
}

bool GameCommander::isAssigned(BWAPI::Unit unit) const
{
	return _scout_units.contains(unit);  //_combat_units.contains(unit) ||
}

BWAPI::Unit GameCommander::getClosestWorkerToTarget(BWAPI::Position target)
{
	BWAPI::Unit closest_unit = nullptr;
	int closest_dist {9999};

	// TODO fetch from container of workermanagement?
	for (const auto & unit : BWAPI::Broodwar->self()->getUnits())
	{
		if (!isAssigned(unit) && unit->getType().isWorker())//&& WorkerManager::Instance().isFree(unit))
		{
			int dist = unit->getDistance(target);
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