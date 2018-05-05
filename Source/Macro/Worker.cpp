#include "Worker.h"

using namespace MagBot;

Worker::Worker()
{	
	for (auto & unit : BWAPI::Broodwar->getAllUnits())
	{
		BWAPI::TilePosition base = BWAPI::Broodwar->self()->getStartLocation();
		if (unit->getType().isMineralField()
			&& unit->getDistance(BWAPI::Position(base)) < 300)
		{
			_mineral_workers_count[unit];
		}
	}
}

Worker::~Worker()
{
}

void Worker::update()
{
	// TODO work on queue system + cooperative pathfinding (including collision detection)
	
	for (const auto & unit : BWAPI::Broodwar->self()->getUnits())
	{
		if (!unit->exists() || !unit->isCompleted())
			continue;

		if (unit->getType().isWorker() && !_workers.contains(unit))
		{
			_workers.insert(unit);
		}
	}

	if (Config::DebugInfo::DrawAllInfo)
	{
		BWAPI::Broodwar->drawTextScreen(0, 0, "%s total : %d", 
			BWAPI::Broodwar->self()->getRace().getWorker().c_str(), getWorkers().size(), BWAPI::Text::White);
		
		if (_depot_worker_count.size() == 0)
		{
			BWAPI::Broodwar->drawTextScreen(0, 10, "%s on minerals : %d", 
				BWAPI::Broodwar->self()->getRace().getWorker().c_str(), 0, BWAPI::Text::White);
		}
		else
		{
			uint8_t total_at_depot {0};
			for (auto & depot : _depot_worker_count)
			{
				total_at_depot += depot.second;			
			}
			BWAPI::Broodwar->drawTextScreen(0, 10, "%s on minerals : %d",
				BWAPI::Broodwar->self()->getRace().getWorker().c_str(), total_at_depot, BWAPI::Text::White);
		}
		
		if (_refinery_worker_count.size() == 0)
		{
			BWAPI::Broodwar->drawTextScreen(0, 20, "%s on gas : %d", 
				BWAPI::Broodwar->self()->getRace().getWorker().c_str(), 0, BWAPI::Text::White);
		}
		else
		{
			uint8_t total_on_gas {0};
			for (auto & refinery : _refinery_worker_count)
			{
				total_on_gas += refinery.second;
			}
			BWAPI::Broodwar->drawTextScreen(0, 20, "%s on gas : %d",
				BWAPI::Broodwar->self()->getRace().getWorker().c_str(), total_on_gas, BWAPI::Text::White);
		}		
	}
}

void Worker::removeDestroyedWorker(BWAPI::Unit unit)
{
	if (!unit)
		return;

	clearPreviousTask(unit);
	_workers.erase(unit);
}

enum Worker::WorkerTask Worker::getWorkerTask(BWAPI::Unit unit)
{
	if (!unit)
		return DEFAULT;

	const std::map<BWAPI::Unit, enum WorkerTask>::const_iterator it = _worker_task_map.find(unit);

	if (it != _worker_task_map.end())
	{
		return it->second;
	}
	return DEFAULT;
}

BWAPI::UnitType	Worker::getWorkerBuildingType(BWAPI::Unit unit)
{
	if (!unit) 
		return BWAPI::UnitTypes::None;

	std::map<BWAPI::Unit, BWAPI::UnitType>::iterator it = _worker_building_type_map.find(unit);

	if (it != _worker_building_type_map.end())
	{
		return it->second;
	}
	return BWAPI::UnitTypes::None;
}

void Worker::setWorkerTask(BWAPI::Unit worker, enum WorkerTask task, BWAPI::Unit worker_task)
{
	if (!worker)
		return;

	clearPreviousTask(worker);
	_worker_task_map[worker] = task;

	if (task == MINE)
	{
		_depot_worker_count[worker_task] += 1;
		_workers_depot_map[worker] = worker_task;
	}
	else if (task == GAS)
	{
		// increase the count of workers assigned to this refinery
		_refinery_worker_count[worker_task] += 1;

		// set the refinery the worker is working on
		_workers_refinery_map[worker] = worker_task;

		if (!worker || !worker_task)
		{
			return;
		}

		// if we have issued a command to this unit already this frame, ignore this one
		if (worker->getLastCommandFrame() >= BWAPI::Broodwar->getFrameCount() || worker->isAttackFrame())
		{
			return;
		}

		// get the unit's current command
		BWAPI::UnitCommand currentCommand(worker->getLastCommand());

		// if we've already told this unit to move to this position, ignore this command
		if ((currentCommand.getType() == BWAPI::UnitCommandTypes::Right_Click_Unit)
			&& (currentCommand.getTargetPosition() == worker_task->getPosition()))
		{
			return;
		}

		worker->rightClick(worker_task);
	}
}

void Worker::setWorkerTask(BWAPI::Unit worker, enum WorkerTask task)
{
	if (!worker)
		return;

	clearPreviousTask(worker);
	_worker_task_map[worker] = task;
}

void Worker::setWorkerTask(BWAPI::Unit worker, enum WorkerTask task, BWAPI::UnitType task_unit_type)
{
	if (!worker)
		return;

	clearPreviousTask(worker);
	_worker_task_map[worker] = task;
	
	if (task == BUILD)
	{
		_worker_building_type_map[worker] = task_unit_type;
	}
}

void Worker::clearPreviousTask(BWAPI::Unit unit)
{
	if (!unit)
		return;

	WorkerTask previous_task = getWorkerTask(unit);

	if (previous_task == MINE)
	{
		_depot_worker_count[_workers_depot_map[unit]] -= 1;
		_workers_depot_map.erase(unit);
	}
	else if (previous_task == BUILD)
	{
		_worker_building_type_map.erase(unit);
	}
	else if (previous_task == GAS)
	{
		_refinery_worker_count[_workers_refinery_map[unit]] -= 1;
		_workers_refinery_map.erase(unit);
	}

	_worker_task_map.erase(unit);
}


BWAPI::Unit Worker::getWorkerResource(BWAPI::Unit unit)
{
	if (!unit)
		return nullptr;

	std::map<BWAPI::Unit, BWAPI::Unit>::iterator it;

	if (getWorkerTask(unit) == MINE)
	{
		it = _workers_depot_map.find(unit);
		if (it != _workers_depot_map.end())
		{
			return it->second;
		}
	}
	else if (getWorkerTask(unit) == GAS)
	{
		it = _workers_refinery_map.find(unit);
		if (it != _workers_refinery_map.end())
		{
			return it->second;
		}
	}

	return nullptr;
}

uint8_t Worker::getNumAssignedDepotWorkers(BWAPI::Unit unit)
{
	if (!unit)
		return 0;

	std::map<BWAPI::Unit, uint8_t>::iterator it;

	if (unit->getType().isResourceDepot())
	{
		it = _depot_worker_count.find(unit);

		if (it != _depot_worker_count.end())
		{
			return it->second;
		}
	}
	return 0;
}

uint8_t Worker::getNumAssignedRefineryWorkers(BWAPI::Unit unit)
{
	if (!unit)
		return 0;

	std::map<BWAPI::Unit, uint8_t>::iterator it;

	if (unit->getType().isRefinery())
	{
		it = _refinery_worker_count.find(unit);

		if (it != _refinery_worker_count.end())
		{
			return it->second;
		}
		else
		{
			_refinery_worker_count[unit] = 0;
		}
	}
	return 0;
}