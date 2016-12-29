#include "WorkerManager.h"

using namespace MagBot;
using namespace BWAPI;

WorkerManager::WorkerManager()
{	
}


WorkerManager::~WorkerManager()
{
}

WorkerManager & WorkerManager::Instance()
{
	static WorkerManager instance;
	return instance;
}

void WorkerManager::update()
{
	updateWorkerTask();

	// TODO Move for loop in their respective handle function, to make the conditionals more specific
	for (const auto & unit : _worker.getWorkers())
	{
		// TODO check these conditionals, if can move to init constructor of worker instead and some can keep here for the update
		if (!unit->exists())
			continue;

		// Ignore the unit if it has one of the following status ailments
		if (unit->isLockedDown() || unit->isMaelstrommed() || unit->isStasised())
			continue;

		// Ignore the unit if it is in one of the following states
		if (unit->isLoaded() || !unit->isPowered() || unit->isStuck())
			continue;

		// Ignore the unit if it is incomplete or busy constructing
		if (!unit->isCompleted())// || unit->isConstructing())
			continue;

		if (Config::DebugInfo::DrawAllInfo && Config::DebugInfo::DrawWorkerTaskInfo)
		{
			showDebugWorkerInfo(unit);
		}
		
		handleMineralWorkers(unit);
		handleIdleWorkers(unit);
	}
}

void WorkerManager::updateWorkerTask()
{
	_worker.update();
}

BWAPI::Unit WorkerManager::getBuilder()
{
	BWAPI::Unit closest_mining_worker = nullptr;

	for (const auto & unit : _worker.getWorkers())
	{
		
		// TODO check closest
		//closestMiningWorker = unit;
		//Position pos = unit->getPosition();
		//Broodwar->drawBoxScreen(pos, pos, Colors::Orange, false);// (pos, 10, Colors::Orange, false);
		return unit;
	}
	return closest_mining_worker;
}

BWAPI::Unit WorkerManager::getBuilderClosestTo(BWAPI::TilePosition tile_position)
{
	BWAPI::Unit closest_mining_worker = nullptr;

	// TODO find closestMiningWorker
	// find the closest unit to this one -> so find the worker which is the closest to the pylon
	closest_mining_worker->getClosestUnit();

	if (!closest_mining_worker)
	{
		return getBuilder();
	}
	return closest_mining_worker;
}

// TODO buildWithSpacing(int)
// AND make sure there's is space between buildings (one tile)?

// drawBox the second it is under construction (permanent) instead of updating every frame

// TODO could add int or a type Priority for the BuildOrderManager
bool WorkerManager::build(BWAPI::UnitType unit_type)
{
	// TODO modify to pylons built, that exist then if !canBuildHere then continue, if can build, if cant find then build pylon,
	// TODO fix bug when targetlocation is slightly in the fog of war, it doesn't build it: SOLUTION -> moveBuilder to location before?
	// -> could be even before having enough minerals to optimize build time
	// TODO when selecting a valid targetlocation, make sure it stays there unless theres something blocking the construction
	// TODO FIX targetlocation example : building photon cannons on some of the tiles of a pylon, etc.
	for (const auto & unit : BWAPI::Broodwar->self()->getUnits())
	{
		if (unit->getType() == BWAPI::UnitTypes::Protoss_Pylon || unit_type == BWAPI::UnitTypes::Protoss_Pylon)
		{
			TilePosition pos = unit->getTilePosition();
			TilePosition target_build_location = Broodwar->getBuildLocation(unit_type, pos);

			// TODO fix getBuilderClosestTo
			//BWAPI::Unit builder = getBuilderClosestTo(target_build_location);
			BWAPI::Unit builder = getBuilder();

			if (!builder)
				return false;

			if (Broodwar->canBuildHere(target_build_location, unit_type, builder))
			{
				_worker.setWorkerTask(builder, Worker::WorkerTask::BUILD, unit_type);
				//Broodwar->drawBoxMap(Position(target_build_location), Position(target_build_location + unitType.tileSize()), Colors::Green);
				
				if (Config::DebugInfo::DrawAllInfo)
				{
					Broodwar->registerEvent([target_build_location, unit_type](Game*)
					{ Broodwar->drawBoxMap(Position(target_build_location),
					Position(target_build_location + unit_type.tileSize()), Colors::Green); },   // action
					nullptr,    // condition
					42);  // frames to run
				}		
				
				builder->build(unit_type, target_build_location);
				// TODO check return here or after the condition
				return true;

			}		
			// TODO check return here or inside the condition
			// return;
		}
	}
	return false;
}

void WorkerManager::showDebugWorkerInfo(BWAPI::Unit unit)
{
	Position pos = unit->getPosition();

	if (_worker.getWorkerTask(unit) == Worker::WorkerTask::MINE)
	{		
		BWAPI::Broodwar->drawTextMap(pos, "MINER");
	}
	else if (_worker.getWorkerTask(unit) == Worker::WorkerTask::BUILD)
	{
		BWAPI::Broodwar->drawTextMap(pos, "BUILDER");
	}
}

void WorkerManager::handleMineralWorkers(BWAPI::Unit unit)
{
	if (unit->isIdle() && _worker.getWorkerTask(unit) == Worker::WorkerTask::MINE) // TO UNCOMMENT:  // 
	{
		// Order workers carrying a resource to return them to the center,
		// otherwise find a mineral patch to harvest.
		if (unit->isCarryingGas() || unit->isCarryingMinerals())
		{
			unit->returnCargo();
		}
		else if (!unit->getPowerUp())  // The worker cannot harvest anything if it
		{                             // is carrying a powerup such as a flag
			// Harvest from the nearest mineral patch or gas refinery
			if (!unit->gather(unit->getClosestUnit(BWAPI::Filter::IsMineralField || BWAPI::Filter::IsRefinery)))
			{
				// If the call fails, then print the last error message
				Broodwar << Broodwar->getLastError() << std::endl;
			}
		}
	}
}

void WorkerManager::handleIdleWorkers(BWAPI::Unit unit)
{
	for (const auto & unit : _worker.getWorkers())
	{
		if (_worker.getWorkerTask(unit) == Worker::WorkerTask::BUILD && unit->isIdle())
		{
			_worker.setWorkerTask(unit, Worker::WorkerTask::MINE);
		}
	}
}


/*void WorkerManager::addWorker()
{
	
}*/

// other managers that need workers call this when they're done with a unit
/*void WorkerManager::finishedWithWorker(BWAPI::Unit unit)
{
	//UAB_ASSERT(unit != nullptr, "Unit was null");
	if (!unit)
		return;
	//BWAPI::Broodwar->printf("BuildingManager finished with worker %d", unit->getID());
	if (_worker.getWorkerTask(unit) != Worker::Scout)
	{
		_worker.setWorkerTask(unit, Worker::Idle);
	}
}*/

