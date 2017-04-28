#include "WorkerManager.h"

using namespace MagBot;

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
	updateWorkerCount();

	// TODO Move for loop in their respective handle function, to make the conditionals more specific
	for (const auto & worker : _worker.getWorkers())
	{
		// TODO check these conditionals, if can move to init constructor of worker instead and some can keep here for the update
		// TODO may have to uncomment if worker isConstruction since we want to be able to have multiple builders at the same time
		/*if (!worker->exists())
			continue;

		// Ignore the unit if it has one of the following status ailments
		if (worker->isLockedDown() || worker->isMaelstrommed() || worker->isStasised())
			continue;

		// Ignore the unit if it is in one of the following states
		if (worker->isLoaded() || !worker->isPowered() || worker->isStuck())
			continue;

		// Ignore the unit if it is incomplete or busy constructing
		if (!worker->isCompleted() || worker->isConstructing())
			continue;
		*/

		if (!worker->isCompleted()) // || worker->isConstructing() TODO may have to remove isConstructing or try to redisplay MINER, etc.
			continue;

		if (worker->isIdle()
			&& _worker.getWorkerTask(worker) != Worker::BUILD
			&& _worker.getWorkerTask(worker) != Worker::SCOUT)
		{
			_worker.setWorkerTask(worker, Worker::IDLE);
		}

		// ************** IN PROGRESS **************

		// TODO change logic, find the assigned unit (refinery to the worker)
		// TODO add function to force assignment to refinery? -> get 3 closest mineral worker and isnt carrying minerals
		
		if (_worker.getWorkerTask(worker) == Worker::GAS)
		{
			BWAPI::Unit refinery = _worker.getWorkerResource(worker);

			// if the refinery doesn't exist anymore
			if (!refinery || !refinery->exists() || refinery->getHitPoints() <= 0)
			{
				BWAPI::Unit depot = getClosestDepot(worker);
				if (depot)
				{
					_worker.setWorkerTask(worker, Worker::MINE, depot);
				}			
			}
		}

		if (Config::DebugInfo::DrawAllInfo && Config::DebugInfo::DrawWorkerTaskInfo)
		{
			showDebugWorkerInfo(worker);
		}
		
	}

	handleGasWorkers();
	handleIdleWorkers();
	handleMineralWorkers();
}

void WorkerManager::updateWorkerCount()
{
	_worker.update();
}

// TODO : FIX BUG multiple gateways is built, probe task glitch, check !test_location.isValid() ?

// whats happening, when enough minerals, closest mining worker is set to builder and wants to build, but a worker set to mine
// also wants to build it

// may be related to the fact that when getbuild gets called, the closest mining worker is set to Builder
// then if the test location isn't valid, we go to the next closest mining worker?
// Possible solution : add reserved tiles? getValidBuildingLocation when doing test_location in BuildingManager

// this problem occurs in Benzene 1.1 top right location
BWAPI::Unit WorkerManager::getBuilder(Building & building)
{
	BWAPI::Unit closest_mining_worker = nullptr;
	int closest_mining_distance {9999};

	for (const auto & worker : _worker.getWorkers())
	{
		if (_worker.getWorkerTask(worker) == Worker::SCOUT || _worker.getWorkerTask(worker) == Worker::GAS)
			continue;

		// this seems to fix the problem above, but WHY?
		if (worker->isCarryingMinerals())
			continue;

		if (_worker.getWorkerTask(worker) == Worker::MINE)
		{
			int distance = worker->getDistance(BWAPI::Position(building._final_position));
			if (!closest_mining_worker || distance < closest_mining_distance)
			{
				closest_mining_worker = worker;
				closest_mining_distance = distance;
			}
		}	
	}
	
	if (closest_mining_worker != nullptr)
	{
		_worker.setWorkerTask(closest_mining_worker, Worker::BUILD, building._unit_type);
	}

	return closest_mining_worker;
}

// TODO finish this function
// Can be used with a Building _final_position tile position
BWAPI::Unit WorkerManager::getBuilderClosestTo(BWAPI::TilePosition tile_position)
{
	BWAPI::Unit closest_mining_worker = nullptr;
	int closest_mining_distance {9999};

	// find the closest unit to this one -> so find the worker which is the closest to the pylon
	for (const auto & worker : _worker.getWorkers())
	{
		if (_worker.getWorkerTask(worker) == Worker::SCOUT || _worker.getWorkerTask(worker) == Worker::GAS)
			continue;

		if (_worker.getWorkerTask(worker) == Worker::MINE)
		{
			int distance = worker->getDistance(BWAPI::Position(tile_position));
			if (!closest_mining_worker || distance < closest_mining_distance)
			{
				closest_mining_worker = worker;
				closest_mining_distance = distance;
			}
		}
	}

	if (!closest_mining_worker)
	{
		//return getBuilder();
	}
	return closest_mining_worker;
}

// TODO buildWithSpacing(int)
// AND make sure there's is space between buildings (one tile)?
// procedure: check if tileposition available and valid, if valid but occupied by building/unit, move unit or get tile size of building
// then add extra tile for spacing then build.


// TODO could add int or a type Priority for the BuildOrderManager
/*bool WorkerManager::build(BWAPI::UnitType unit_type)
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
				_worker.setWorkerTask(builder, Worker::BUILD, unit_type);
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
}*/

void WorkerManager::showDebugWorkerInfo(const BWAPI::Unit & worker)
{
	BWAPI::Position pos = worker->getPosition();

	BWAPI::Broodwar->drawTextMap(pos, "%s", getWorkerTaskName(worker).c_str());
}

// TODO call public function from worker.h
void WorkerManager::handleMineralWorkers()
{
	for (const auto & worker : _worker.getWorkers())
	{
		if (worker->isIdle() && _worker.getWorkerTask(worker) == Worker::MINE) // TO UNCOMMENT:  // 
		{
			// Order workers carrying a resource to return them to the center,
			// otherwise find a mineral patch to harvest.
			if (worker->isCarryingMinerals())
			{
				worker->returnCargo();
			}
			// TODO Comment or remove after cooperative pathfinding + queue system algorithms is completed
			else if (!worker->getPowerUp())  // The worker cannot harvest anything if it
			{                             // is carrying a powerup such as a flag
				// Harvest from the nearest mineral patch or gas refinery

				if (!worker->gather(worker->getClosestUnit(BWAPI::Filter::IsMineralField)))
				{
					//TODO send workers to next mineral lines
					// If the call fails, then print the last error message
					BWAPI::Broodwar << BWAPI::Broodwar->getLastError() << std::endl;
				}
			}
		}
	}
}

void WorkerManager::handleGasWorkers()
{
	for (const auto & unit : BWAPI::Broodwar->self()->getUnits())
	{
		// if that unit is a refinery
		if (unit->getType().isRefinery() && unit->isCompleted())
		{
			uint8_t num_assigned = _worker.getNumAssignedRefineryWorkers(unit);

			// if it's less than we want it to be, fill up
			for (uint8_t i {0}; i < (3 - num_assigned); ++i)
			{
				BWAPI::Unit gas_worker = getGasWorker(unit);
				if (gas_worker)
				{
					_worker.setWorkerTask(gas_worker, Worker::GAS, unit);
				}
			}
		}
	}

	/*if (unit->isIdle() && _worker.getWorkerTask(unit) == Worker::GAS)
	{
		if (unit->isCarryingGas())
		{
			unit->returnCargo();
		}
		else if (!unit->getPowerUp())
		{
			if (!unit->gather(unit->getClosestUnit(BWAPI::Filter::IsRefinery)))
			{
				Broodwar << Broodwar->getLastError() << std::endl;
			}
		}
	}*/
}

void WorkerManager::handleIdleWorkers()
{
	for (const auto & worker : _worker.getWorkers())
	{
		if (_worker.getWorkerTask(worker) == Worker::IDLE) // (_worker.getWorkerTask(worker) == Worker::BUILD && worker->isIdle())
		{
			BWAPI::Unit depot = getClosestDepot(worker);
			if (depot)
			{
				_worker.setWorkerTask(worker, Worker::MINE, depot);
			}
		}
	}
}

BWAPI::Unit WorkerManager::getGasWorker(BWAPI::Unit refinery)
{
	BWAPI::Unit closest_worker = nullptr;	
	int closest_distance {9999};

	for (const auto & worker : _worker.getWorkers())
	{
		// if miner is carrying minerals, don't select him
		if (worker->isCarryingMinerals())
			continue;

		if (_worker.getWorkerTask(worker) == Worker::MINE)
		{	
			int distance = worker->getDistance(refinery);
			if (!closest_worker || distance < closest_distance)
			{
				closest_worker = worker;
				closest_distance = distance;
			}
		}
	}
	return closest_worker;
}

BWAPI::Unit WorkerManager::getClosestDepot(BWAPI::Unit worker)
{
	BWAPI::Unit closest_depot = nullptr;
	int closest_distance {9999};

	for (const auto & unit : BWAPI::Broodwar->self()->getUnits())
	{
		if (unit->getType().isResourceDepot() && unit->isCompleted())
		{
			int distance = unit->getDistance(worker);
			if (!closest_depot || distance < closest_distance)
			{
				closest_depot = unit;
				closest_distance = distance;
			}
		}
	}
	return closest_depot;
}

void WorkerManager::setWorkerScout(BWAPI::Unit worker)
{
	_worker.setWorkerTask(worker, Worker::SCOUT);
}

std::string WorkerManager::getWorkerTaskName(BWAPI::Unit worker)
{
	if (!worker)
		return "UNKNOWN";

	Worker::WorkerTask task_value = _worker.getWorkerTask(worker);
	if (task_value == Worker::MINE)
	{
		return "MINER";
	}
	else if (task_value == Worker::GAS)
	{
		return "GAS";
	}
	else if (task_value == Worker::BUILD)
	{
		return "BUILDER";
	}
	else if (task_value == Worker::SCOUT)
	{
		return "SCOUT";
	}
	return "UNKNOWN";
}

void WorkerManager::setWorkerFree(BWAPI::Unit worker)
{
	if (_worker.getWorkerTask(worker) != Worker::SCOUT)
	{
		BWAPI::Unit depot = getClosestDepot(worker);
		if (depot)
		{
			_worker.setWorkerTask(worker, Worker::MINE, depot);
		}
	}
}

void WorkerManager::onUnitDestroy(BWAPI::Unit unit)
{
	if (!unit)
		return;

	if (unit->getType().isWorker() && unit->getPlayer() == BWAPI::Broodwar->self())
	{
		_worker.removeDestroyedWorker(unit);
	}
}