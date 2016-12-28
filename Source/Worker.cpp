#include "Worker.h"

using namespace MagBot;
using namespace BWAPI;

Worker::Worker()
{
	
}


Worker::~Worker()
{
}

void Worker::update()
{
	// TODO try to shorten the verification of all units
	for (auto & unit : BWAPI::Broodwar->self()->getUnits())
	{
		if (!unit->exists() || !unit->isCompleted())
			continue;

		if (unit->getType().isWorker() && !_workers.contains(unit))
		{
			setWorkerTask(unit, Mine);
			_workers.insert(unit);
		}
	}

	removeDestroyedWorker();

	if (Config::DebugInfo::DrawAllInfo)
	{
		Broodwar->drawTextScreen(0, 10, "%s : %d", Broodwar->self()->getRace().getWorker().c_str(), getWorkers().size(), Text::White);
	}	
}

void Worker::removeDestroyedWorker()
{
	for (auto & unit : getWorkers())
	{
		if (!unit->exists())
		{
			clearPreviousTask(unit);
			_workers.erase(unit);
		}
	}
}

enum Worker::WorkerTask Worker::getWorkerTask(BWAPI::Unit unit)
{
	if (!unit)
		return Default;

	const std::map<BWAPI::Unit, enum WorkerTask>::const_iterator it = _workerTaskMap.find(unit);

	if (it != _workerTaskMap.end())
	{
		return it->second;
	}

	return Default;
}

BWAPI::UnitType	Worker::getWorkerBuildingType(BWAPI::Unit unit)
{
	if (!unit) { return BWAPI::UnitTypes::None; }

	std::map<BWAPI::Unit, BWAPI::UnitType>::iterator it = _workerBuildingTypeMap.find(unit);

	if (it != _workerBuildingTypeMap.end())
	{
		return it->second;
	}

	return BWAPI::UnitTypes::None;
}

void Worker::setWorkerTask(BWAPI::Unit unit, enum WorkerTask task)
{
	if (!unit)
		return;

	clearPreviousTask(unit);

	if (task == Mine)
	{
		_workerTaskMap[unit] = task;
	}
	else if (task == Build)
	{
		_workerTaskMap[unit] = task;
	}
}

void Worker::setWorkerTask(BWAPI::Unit unit, enum WorkerTask task, BWAPI::UnitType taskUnitType)
{
	if (!unit)
		return;

	clearPreviousTask(unit);
	_workerTaskMap[unit] = task;
	
	if (task == Build)
	{
		_workerBuildingTypeMap[unit] = taskUnitType;
	}
	// TODO add building task to a priority queue?	
}

// TODO could optimize, instead of erase and insert, do copy/swap and insert
void Worker::clearPreviousTask(BWAPI::Unit unit)
{
	if (!unit)
		return;

	WorkerTask previousTask = getWorkerTask(unit);

	if (previousTask == Build)
	{
		_workerBuildingTypeMap.erase(unit);
	}

	_workerTaskMap.erase(unit);

	/*if (previousTask == Mine)
	{
		_workerTaskMap.erase(unit);
	}
	else if (previousTask == Build)
	{
		_workerTaskMap.erase(unit);
	}*/

}


// Ignore the unit if it no longer exists
// Make sure to include this block when handling any Unit pointer!
/*if (!u->exists())
continue;

// Ignore the unit if it has one of the following status ailments
if (u->isLockedDown() || u->isMaelstrommed() || u->isStasised())
continue;

// Ignore the unit if it is in one of the following states
if (u->isLoaded() || !u->isPowered() || u->isStuck())
continue;

// Ignore the unit if it is incomplete or busy constructing
if (!u->isCompleted() || u->isConstructing())
continue;
*/

// Finally make the unit do some stuff!


// If the unit is a worker unit
/*if (u->getType().isWorker())
{
	// if our worker is idle
	if (u->isIdle())
	{
		// Order workers carrying a resource to return them to the center,
		// otherwise find a mineral patch to harvest.
		if (u->isCarryingGas() || u->isCarryingMinerals())
		{
			u->returnCargo();
		}
		else if (!u->getPowerUp())  // The worker cannot harvest anything if it
		{                             // is carrying a powerup such as a flag
			// Harvest from the nearest mineral patch or gas refinery
			if (!u->gather(u->getClosestUnit(IsMineralField || IsRefinery)))
			{
				// If the call fails, then print the last error message
				Broodwar << Broodwar->getLastError() << std::endl;
			}

		} // closure: has no powerup

*/
		// step 1: [Completed] init WorkerManager


		// step 2: init ConstructorManager


		// step 3: [Completed] add all workers units not in worker manager container every frame OR when you build a new worker -> add in WorkerM


		// step 4: getOneFreeWorker -> that is mining/closest to the nexus, check if hasMineral too, etc. to optimize


		// step 5: setWorker in WorkerManager to BUILD enum? then add him in ConstructorManager,  
		// *TODO CASE if is mining minerals/gas, check if dead then remove from WorkerManager

		// step 6: in ConstructorManager, setWorkerTask(UnitType Building -> gateway)

		// step 7: make pylons structure to keep track of their TilePosition, check if pylon exists 

		// step 8: getPositionBuilding(Building -> Pylon)

		// step 9: isBuildable() or canBuildHere()? 

		// step 10: verify minerals, etc. then constructor -> build(gateway, posBuildable)

		// step 11: send back to worker manager and set him to Mine Minerals enum?

		// step 12: create update() function to the constructor and worker manager



		/*
		// TODO create instance of constructor and select from worker manager, then build a gateway
		if (u->isCarryingMinerals())
		{
		// try to build gateway near a pylon
		if (BWAPI::Broodwar->self()->minerals() >= 150) //BWAPI::Broodwar->self()->supplyUsed() >= 8 &&
		{
		Position pos = u->getPosition();
		Error lastErr = Broodwar->getLastError();
		Broodwar->registerEvent([pos, lastErr](Game*){ Broodwar->drawTextMap(pos, "test", Text::White, lastErr.c_str()); },   // action
		nullptr,    // condition
		Broodwar->getLatencyFrames());  // frames to run

		UnitType _gateway = BWAPI::UnitTypes::Protoss_Gateway;
		Unit supplyBuilder = u->getClosestUnit(GetType == _gateway.whatBuilds().first &&
		(IsIdle || IsGatheringMinerals) &&
		IsOwned);
		TilePosition targetBuildLocation = Broodwar->getBuildLocation(_gateway, supplyBuilder->getTilePosition());


		Broodwar->registerEvent([targetBuildLocation, _gateway, supplyBuilder](Game*)
		{
		Broodwar->drawCircleScreen(BWAPI::Position(supplyBuilder->getTilePosition().x, supplyBuilder->getTilePosition().y), 5, BWAPI::Colors::Green);

		Broodwar->drawBoxMap(Position(targetBuildLocation),
		Position(targetBuildLocation + _gateway.tileSize()),
		Colors::Blue);
		},
		nullptr,  // condition
		_gateway.buildTime() + 100);  // frames to run

		supplyBuilder->build(_gateway, targetBuildLocation);

		}

		} // end of TODO
		*/




/*

	} // closure: if idle

}
*/