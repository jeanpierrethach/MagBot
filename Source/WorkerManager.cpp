#include "WorkerManager.h"

using namespace MagBot;

WorkerManager::WorkerManager()
{	
	for (auto & unit : BWAPI::Broodwar->getAllUnits())
	{
		BWAPI::TilePosition base = BWAPI::Broodwar->self()->getStartLocation();
		if (unit->getType().isMineralField()
			&& unit->getDistance(BWAPI::Position(base)) < 300)
		{
			_mineral_nodes.insertMineralPatch(unit);
			//_mineral_nodes._deque_workers.insertMineralPatch(unit);
		}
	}

	//_mineral_nodes.initializeMineralPatches();
	_mineral_nodes.initializePatch();
	//_deque_workers.initializePatch();
	//BWAPI::Broodwar->sendText("mineral_patch_count: %d", _mineral_nodes.getMineralPatch().size());
	//BWAPI::Broodwar->sendText("mineral nodes count: %d", _mineral_nodes.getMineralNodes().size());
	//BWAPI::Broodwar->sendText("deque size: %d", _mineral_nodes.getMineralPatch().size());
	/*for (auto & patch : _mineral_nodes.getMineralPatch())
	{
		BWAPI::Broodwar->sendText("deque id: %d", patch.first);
	}*/
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

	// TODO : time the execution, sequential and parallel methods
	// make acceleration graphs
	// + results
	optimizeWorkersMining();
	//handleMineralWorkers();
	/*if (Config::DebugInfo::DrawMineralHandling)
	{
		showDebugMineralHandling();
	}*/

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

	WorkerState worker_state = _mineral_nodes.getWorkerState(worker);

	std::string state = "";
	if (worker_state == WorkerState::MOVING_TO_PATCH)
	{
		state = "MOVING_TO_PATCH";
	} 
	else if (worker_state == WorkerState::MINING)
	{
		state = "MINING";
	}
	else if (worker_state == WorkerState::NONE)
	{
		state = "MOVING_TO_DEPOT";
	}
	BWAPI::Broodwar->drawTextMap(pos, "%s : %s", getWorkerTaskName(worker).c_str(), state.c_str());
}

// TODO optimize mineral assignment per worker
// steps: instantiate queues for each mineral nodes
// set optimal nb of workers per mineral nodes
// calculate optimal node for each worker (depending on others)
// send worker to queue's node

// TODO start of game, assign ids to mineral nodes next to starting base
// then add into the vector of deques

// loop over mineral nodes and for each id, insert an optimal worker for its node position
// create mineral node class

// https://github.com/MartinRooijackers/LetaBot/tree/master/Research/MineralGatheringAlgorithm
void WorkerManager::optimizeWorkersMining()
{
	// source : http://wiki.teamliquid.net/starcraft/Mining

	_mineral_nodes.displayWorkerinDeque();

	for (const auto & worker : _worker.getWorkers())
	{
		// TODO : when worker state is mining -> need to recalculate
		// optimal patch
		/*if (_worker.getWorkerTask(worker) == Worker::MINE)
		{
			if (isAssignedToPatch(worker))
			{
				// recalculate
			}
		}*/

		// TODO logic 
		// Gather built-in switching problems
		// call gather on every frame for the assigned patch
		//


		// CASES:
		// if idle then state = MOVING_TO_PATCH
		//   and add to queue
		// if carryingMin then state = MOVING_TO_DEPOT
		//   and remove from queue
		//
		WorkerState state = _mineral_nodes.getWorkerState(worker);

		BWAPI::Order mining_order {BWAPI::Orders::MiningMinerals};
		BWAPI::Order order = worker->getOrder();
		// TODO
		// CASE: if worker isGatheringMinerals
		// worker->isGatheringMinerals() not good
		// https://bwapi.github.io/class_b_w_a_p_i_1_1_order.html
		// could use Order Class for MoveToMinerals, WaitForMinerals
		// ReturnMinerals, MiningMinerals
		if (state == WorkerState::MOVING_TO_PATCH &&
			order == mining_order && _worker.getWorkerTask(worker) == Worker::MINE)
		{
			int frame = BWAPI::Broodwar->getFrameCount();
			_mineral_nodes.setFrameStartMining(worker, frame);
			_mineral_nodes.setWorkerState(worker, WorkerState::MINING);
		}

		// TODO test
		// make sure worker mine the correct patch
		if (state == WorkerState::MINING
			&& !worker->isCarryingMinerals() && _worker.getWorkerTask(worker) == Worker::MINE)
		{
			BWAPI::Unit assigned_patch = _mineral_nodes.getAssignedMineralPatch(worker);
			worker->rightClick(assigned_patch);
			continue;
		}

		if (state == WorkerState::MINING && 
			worker->isCarryingMinerals() && _worker.getWorkerTask(worker) == Worker::MINE)
		{
			// TODO find solution
			// since we remove worker from deque
			// can't verify state == WorkerState::MOVING_TO_DEPOT
			_mineral_nodes.removeWorkerFromPatch(worker);
			//_mineral_nodes.setWorkerState(worker, WorkerState::MOVING_TO_DEPOT);
			//BWAPI::Broodwar->sendText("state: MOVING TO DEPOT");
		}


		if (state == WorkerState::NONE && //state == WorkerState::MOVING_TO_DEPOT &&
			!worker->isCarryingMinerals() && _worker.getWorkerTask(worker) == Worker::MINE)
		{
			calculateBestPatch(worker);

			//_mineral_nodes.setWorkerState(worker, WorkerState::MOVING_TO_PATCH);
			//BWAPI::Broodwar->sendText("state: MOVING TO PATCH");
		}

		// TODO:
		// WILL ONLY BE CALLED ONCE when probe is created
		// make it as a function so can be reused and either
		// be called here to onCreated (for probe unit)
		if (worker->isIdle() && _worker.getWorkerTask(worker) == Worker::MINE)
		{
			// THIS SEEMS NOT ENTERING since worker will never be idle again
			// because of automatic call of returnCargo for right click
			// or gather
			if (worker->isCarryingMinerals())
			{
				//worker->returnCargo();
			} 
			else 
			{
				calculateBestPatch(worker);
			}

		}
		/*if (!worker->isGatheringMinerals())
		{
			BWAPI::Position base = InformationManager::getStartingBaseLocation();
			int dist = worker->getDistance(base);
			BWAPI::UnitType probe = BWAPI::UnitTypes::Protoss_Probe;
			double ms = probe.topSpeed();

			// get mindist between each patch - worker

			// check if current optimal patch isn't already occupied/full


		}*/
	}
}

void WorkerManager::calculateBestPatch(BWAPI::Unit worker)
{
	// TODO if best patch according to algorithm
	// then if that patch already has 2 workers then get
	// other best patch

	// iterate over all workers mining
	// get remaining mining frames for each worker

	// calculate distance in frames for current position and
	// patch + patch to depot

	// assign best patch which minimize 
	// frames to the current worker

	// search for mineral patch queue
	const int MINING_TIME = 80;
	
	BWAPI::Unit best_patch = nullptr;
	int total_work = 0;

	// calculate total work for each deque and minimize
	for (auto d : _mineral_nodes._deque_workers)
	{
		int total_work_deque = 0;

		if (d.deque.size() > 2)
			continue;

		for (auto w : d.deque)
		{
			// not started mining
			if (w.frame_start_mining == 0)
			{	
				int distance = w.worker->getDistance(d.patch);
				total_work_deque += distance + MINING_TIME;
			} 
			else
			{
				int current_frame = BWAPI::Broodwar->getFrameCount();
				total_work_deque += MINING_TIME - (current_frame - w.frame_start_mining);
			}
		}

		//BWAPI::Broodwar->sendText("total_work_deque: %d", total_work_deque);

		// calculate work for current worker searching for patch
		int work_current_worker = 0;
		BWAPI::Position base = InformationManager::getStartingBaseLocation();
		
		work_current_worker += worker->getDistance(d.patch) + MINING_TIME
			+ d.patch->getDistance(base);		

		int work = total_work_deque + work_current_worker;

		if (!best_patch || work < total_work)
		{
			best_patch = d.patch;
			total_work = work;
		}
	}

	int frame_start_moving = BWAPI::Broodwar->getFrameCount();
	WorkerMining worker_mining(0, frame_start_moving, best_patch, worker); // closest_patch

	worker_mining.state = WorkerState::MOVING_TO_PATCH;
	
	int patch_id = best_patch->getID();
	_mineral_nodes.insertWorkerToPatch(worker_mining, patch_id);

	worker->gather(best_patch);
}

/*void WorkerManager::showDebugMineralHandling()
{
	for (auto deque : _mineral_nodes.getMineralPatch())
	{
		int count = 0;
		for (auto worker : deque.second)
		{
			BWAPI::Unit unit = worker.getWorker();
			int patch_id = worker.getMineralPatchID();
			count++;
			BWAPI::Broodwar->drawTextMap(BWAPI::Position(360, 50 + (count * 10)), "%d, %d", unit->getID(), patch_id);
		}
	}
}*/


// TODO call public function from worker.h
void WorkerManager::handleMineralWorkers()
{
	for (const auto & worker : _worker.getWorkers())
	{
		if (worker->isIdle() && _worker.getWorkerTask(worker) == Worker::MINE)
		{
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
		if (unit->getType().isRefinery() && unit->isCompleted())
		{
			uint8_t num_assigned = _worker.getNumAssignedRefineryWorkers(unit);

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
}

void WorkerManager::handleIdleWorkers()
{
	for (const auto & worker : _worker.getWorkers())
	{
		if (_worker.getWorkerTask(worker) == Worker::IDLE)
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