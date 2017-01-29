#include "ScoutManager.h"

using namespace MagBot;


ScoutManager::ScoutManager()
{
}


ScoutManager::~ScoutManager()
{
}

// TODO Release scout after his task is done? or keep moving around, add gas steal?
// TODO bug to fix when getBuilder from workermanager, it can take this scout unit
// TODO add info about enemy race, etc... (scouting information in information manager)

void ScoutManager::update()
{
	if (!_worker_scout || !_worker_scout->exists() || !(_worker_scout->getHitPoints() > 0))
	{
		return;
	}
	//int scout_hp = _worker_scout->getHitPoints() + _worker_scout->getShields();

	BWTA::BaseLocation * enemy_base_location = nullptr;

	if (!enemy_base_location)
	{
		for (BWTA::BaseLocation * start_location : BWTA::getStartLocations())
		{
			BWAPI::TilePosition start_location_tilepos = start_location->getTilePosition();
			// if we haven't explored it yet
			if (!BWAPI::Broodwar->isExplored(start_location_tilepos))
			{
				
				if (!_worker_scout || !start_location_tilepos.isValid())
				{
					return;
				}

				// if we have issued a command to this unit already this frame, ignore this one
				if (_worker_scout->getLastCommandFrame() >= BWAPI::Broodwar->getFrameCount() || _worker_scout->isAttackFrame())
				{
					return;
				}

				BWAPI::Broodwar->drawLineMap(_worker_scout->getPosition(), BWAPI::Position(start_location_tilepos), BWAPI::Colors::White);

				// get the unit's current command
				BWAPI::UnitCommand currentCommand(_worker_scout->getLastCommand());

				// if we've already told this unit to move to this position, ignore this command
				if ((currentCommand.getType() == BWAPI::UnitCommandTypes::Move) 
					&& (currentCommand.getTargetPosition() == BWAPI::Position(start_location_tilepos))
					&& _worker_scout->isMoving())
				{
					return;
				}

				// if nothing prevents it, attack the target
				_worker_scout->move(BWAPI::Position(start_location_tilepos));
	
				return;
			}
		}
	}
}


ScoutManager & ScoutManager::Instance()
{
	static ScoutManager instance;
	return instance;
}

void ScoutManager::setWorkerScout(BWAPI::Unit unit)
{
	// if we have a previous worker scout, release it back to the worker manager
	/*if (_worker_scout)
	{
		WorkerManager::Instance().setWorkerFree(_worker_scout);
	}*/

	_worker_scout = unit;
	WorkerManager::Instance().setWorkerScout(_worker_scout);
}
