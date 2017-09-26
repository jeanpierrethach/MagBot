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
	if (_scout_units.empty() && !_initial_scout)
	{
		BWAPI::Unit supply_provider = getFirstSupplyProvider();

		if (supply_provider)
		{
			BWAPI::Unit worker_scout = getClosestWorkerToTarget(supply_provider->getPosition());
			
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

void GameCommander::setCombatUnits()
{
	for (auto unit : BWAPI::Broodwar->self()->getUnits())
	{

	}

	if (_combat_units.empty())
	{
		BWAPI::Unit unit;

		// search for valid units around the zone
		// else search for squads nearby

		if (unit)
		{
			// todo set combat units in class
			DefenceManager::Instance();
		}
	}
}

void GameCommander::assignUnit(BWAPI::Unit unit, BWAPI::Unitset & set)
{
	if (!_scout_units.contains(unit)) 
	{ 
		set.insert(unit); 
	}
}

bool GameCommander::isAssigned(BWAPI::Unit unit) const
{
	return _scout_units.contains(unit);
}

BWAPI::Unit GameCommander::getClosestWorkerToTarget(BWAPI::Position target)
{
	BWAPI::Unit closest_unit = nullptr;
	int closest_dist {9999};

	for (const auto & unit : BWAPI::Broodwar->self()->getUnits())
	{
		if (!isAssigned(unit) && unit->getType().isWorker())
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