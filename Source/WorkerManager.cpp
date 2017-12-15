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
		}
	}
	_mineral_nodes.initializePatch();
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

	for (const auto & worker : _worker.getWorkers())
	{
		// Uncomment when these states will need to be considered
		/*
		if (!worker->exists())
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

		if (!worker->isCompleted())
			continue;

		if (worker->isIdle()
			&& _worker.getWorkerTask(worker) != Worker::BUILD
			&& _worker.getWorkerTask(worker) != Worker::SCOUT)
		{
			_worker.setWorkerTask(worker, Worker::IDLE);
		}

		// TODO add function to force assignment to refinery -> get 3 closest mineral worker and that aren't carrying minerals
		if (_worker.getWorkerTask(worker) == Worker::GAS)
		{
			BWAPI::Unit refinery = _worker.getWorkerResource(worker);

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
	optimizeWorkersMining();

	//handleMineralWorkers();
}

void WorkerManager::updateWorkerCount()
{
	_worker.update();
}

BWAPI::Unit WorkerManager::getBuilder(Building & building)
{
	BWAPI::Unit closest_mining_worker = nullptr;
	int closest_mining_distance {9999};

	for (const auto & worker : _worker.getWorkers())
	{
		if (_worker.getWorkerTask(worker) == Worker::SCOUT || _worker.getWorkerTask(worker) == Worker::GAS)
			continue;

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

void WorkerManager::optimizeWorkersMining()
{
	_mineral_nodes.displayWorkerinDeque();

	for (const auto & worker : _worker.getWorkers())
	{
		WorkerState state = _mineral_nodes.getWorkerState(worker);

		BWAPI::Order mining_order {BWAPI::Orders::MiningMinerals};
		BWAPI::Order order = worker->getOrder();

		if (_worker.getWorkerTask(worker) != Worker::MINE)
			continue;

		if (state == WorkerState::MOVING_TO_PATCH && order == mining_order)
		{
			int frame = BWAPI::Broodwar->getFrameCount();
			_mineral_nodes.setFrameStartMining(worker, frame);
			_mineral_nodes.setWorkerState(worker, WorkerState::MINING);
		}

		// TODO: 12/14/2017 (size() cannot read memory)
		/*if (state == WorkerState::MINING && !worker->isCarryingMinerals())
		{
			BWAPI::Unit assigned_patch = _mineral_nodes.getAssignedMineralPatch(worker);
			worker->rightClick(assigned_patch);
			continue;
		}*/

		if (state == WorkerState::MINING && worker->isCarryingMinerals())
		{
			_mineral_nodes.removeWorkerFromPatch(worker);
		}

		if (state == WorkerState::NONE && !worker->isCarryingMinerals())
		{
			if (Config::TestOptions::ParallelAssignmentMining)
			{
				std::thread first(&WorkerManager::assignBestPatch, this, worker);
				first.join();
			}
			else
			{
				assignBestPatch(worker);
			}
		}

		// This is called once whenever the worker has been sucessfully completed,
		// trained from the depot
		if (worker->isIdle())
		{
			if (Config::TestOptions::ParallelAssignmentMining)
			{
				std::thread second(&WorkerManager::assignBestPatch, this, worker);
				second.join();
			}
			else
			{
				assignBestPatch(worker);
			}			
		}
	}
}

std::pair<BWAPI::Unit, int> WorkerManager::calculateBestPatch(BWAPI::Unit worker, const int begin, const int end)
{
	if (end > _mineral_nodes._deque_workers.size())
	{
		throw std::runtime_error("WorkerManager::calculateBestPatch: Random access out of bounds");
	}

	BWAPI::Unit best_patch = nullptr;
	int total_work = 0;

	// source : http://wiki.teamliquid.net/starcraft/Mining
	const int MINING_TIME = 80;

	for (int i = begin; i < end; i++)
	{
		auto & d = _mineral_nodes._deque_workers[i];

		int total_work_deque = 0;

		if (d.deque.size() > 2)
			continue;

		for (auto & w : d.deque)
		{
			// worker has not started mining
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

		// calculate work for current worker searching for patch
		int work_current_worker = 0;
		BWAPI::Position base = InformationManager::getStartingBaseLocation();

		work_current_worker += worker->getDistance(d.patch) + MINING_TIME
			+ d.patch->getDistance(base);

		int work = total_work_deque + work_current_worker;

		{
			if (!best_patch || work < total_work)
			{
				best_patch = d.patch;
				total_work = work;
			}
		}
	}

	return std::make_pair(best_patch, total_work);
}

std::pair<int, int> WorkerManager::getRangePair(int no_future, int size, int nb_threads)
{
	int inferior_index = no_future * size / nb_threads;
	int superior_index = (no_future + 1) * size / nb_threads - 1;
	
	return std::make_pair(inferior_index, superior_index);
}

void WorkerManager::assignBestPatch(BWAPI::Unit worker)
{	
	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

	BWAPI::Unit best_patch = nullptr;

	if (Config::TestOptions::ParallelAssignmentMining)
	{
		std::vector<std::future<std::pair<BWAPI::Unit, int>> > futures;

		for (int i = 0; i < Config::TestOptions::NbThreads; i++)
		{
			std::pair<int, int> range = getRangePair(i, _mineral_nodes._deque_workers.size(), Config::TestOptions::NbThreads);
			futures.emplace_back(std::async(&WorkerManager::calculateBestPatch, this, worker, range.first, range.second));
		}

		BWAPI::Unit future_patch = nullptr;
		int work = 0;
		for (auto & f : futures)
		{
			auto temp = f.get();
			if (!future_patch || work > temp.second)
			{
				future_patch = temp.first;
				work = temp.second;
			}
		}
		best_patch = future_patch;
	}
	else
	{
		best_patch = calculateBestPatch(worker, 0, _mineral_nodes._deque_workers.size()).first;
	}

	int frame_start_moving = BWAPI::Broodwar->getFrameCount();
	WorkerMining worker_mining(0, frame_start_moving, best_patch, worker);

	worker_mining.state = WorkerState::MOVING_TO_PATCH;
	
	int patch_id = best_patch->getID();

	if (Config::TestOptions::ParallelAssignmentMining)
	{
		_mineral_nodes.insertWorkerToPatchParallel(worker_mining, patch_id);
	}
	else
	{
		_mineral_nodes.insertWorkerToPatch(worker_mining, patch_id);
	}

	worker->gather(best_patch);

	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	InformationManager::Instance().log(worker_mining, duration);
}

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
			else if (!worker->getPowerUp())  // The worker cannot harvest anything if it is carrying a powerup such as a flag
			{
				if (!worker->gather(worker->getClosestUnit(BWAPI::Filter::IsMineralField)))
				{
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