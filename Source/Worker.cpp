#include "Worker.h"

using namespace MagBot;

Worker::Worker()
{	
	for (auto & unit : BWAPI::Broodwar->getAllUnits())
	{
		BWAPI::TilePosition base = BWAPI::Broodwar->self()->getStartLocation();
		if (unit->getType() == BWAPI::UnitTypes::Resource_Mineral_Field 
			&& unit->getDistance(BWAPI::Position(base)) < 300)
		{
			_mineral_workers_count[unit] = 0;
		}
	}
}

Worker::~Worker()
{
}

void Worker::update()
{
	// TODO handle minerals workers with Queue system and cooperative pathfinding algorithms
	for (auto & mineral_patch : _mineral_workers_count)
	{
		BWAPI::Broodwar->drawTextMap(mineral_patch.first->getPosition(), "#%d", mineral_patch.first->getID());
		if (mineral_patch.first->isBeingGathered())
		{
			BWAPI::Broodwar->drawCircleMap(mineral_patch.first->getPosition(), 30, BWAPI::Colors::Black, true);
		}
	}
	// step 1 calculate travel distance from position worker to mineral field assigned

	// step 2 calculate time gathering

	// step 3 calculate time travel distance from mineral patch to return cargo

	// assign worker to closest mineral field related to its position
	// no more than 2 workers on one mineral patch


	// position from worker to mineral field

	// TODO try to shorten the verification of all units
	// For every worker constructed or existing, add it to the container
	for (const auto & unit : BWAPI::Broodwar->self()->getUnits())
	{
		if (!unit->exists() || !unit->isCompleted())
			continue;

		if (unit->getType().isWorker() && !_workers.contains(unit))
		{
			_workers.insert(unit);
		}
	}

// *************	UNDER WORK (Cooperative pathfinding + queue system algorithms    ********************************

	// TODO calculate best mineral patch to assign for worker when returned cargo, calculate by distance, time travel, speed, etc..
	/*
	for (auto & worker : _workers)
	{
		auto & iterator = _workers_mineral_patch.begin();
		//for (; iterator != _workers_mineral_patch.end(); ++iterator)
		//{
			_worker_mineral_assignment[iterator->first].push(worker);
		//}
		//if (_worker_mineral_assignment.find(worker) == _worker_mineral_assignment.end())
		//{
			
		//}	
		//Broodwar->sendText("%d", _worker_mineral_assignment..size());

	}

	// http://stackoverflow.com/questions/16096756/maps-holding-queues-using-vs-insert
	
	for (auto & worker : _worker_mineral_assignment)
	{
		for (size_t i(0); i < worker.second.size(); ++i)
		{
			if (worker.first->isBeingGathered())
			{
				//Broodwar->sendText("#%d being gathered", worker.first->getID());
				//TODO move second worker at full speed and braking when first worker almost done
			}
			else
			{
				if (worker.second.front()->isIdle() && getWorkerTask(worker.second.front()) == Worker::MINE)
				{
					worker.second.front()->gather(worker.first);
					
					if (worker.second.front()->isCarryingMinerals())
					{
						worker.second.pop();
					}
				}
				//Broodwar->sendText("#%d NOT being gathered", worker.first->getID());
				
			}	
		}
	}
	*/
// ************************************************************************************************************************

	removeDestroyedWorker();

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
			int total_at_depot = 0;
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
			int total_on_gas = 0;
			for (auto & refinery : _refinery_worker_count)
			{
				total_on_gas += refinery.second;
			}
			BWAPI::Broodwar->drawTextScreen(0, 20, "%s on gas : %d",
				BWAPI::Broodwar->self()->getRace().getWorker().c_str(), total_on_gas, BWAPI::Text::White);
		}		
	}
}

void Worker::removeDestroyedWorker()
{
	for (const auto & unit : getWorkers())
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

	// ********** IN PROGRESS ************
	if (task == MINE)
	{
		_depot_worker_count[worker_task] += 1;
		_workers_depot_map[worker] = worker_task;

		// TODO get the closest mineral patch available BWAPI::Unit mineral_patch
		// TODO use _worker_mineral_assignment or std::map<Unit, Unit> ? worker / mineral_patch
		// TODO increment nb of workers in the mineral patch assigned
		// send worker to gather that mineral patch
	}
	// ********** IN PROGRESS ************

	else if (task == GAS)
	{
		// increase the count of workers assigned to this refinery
		_refinery_worker_count[worker_task] += 1;

		// set the refinery the worker is working on
		_workers_refinery_map[worker] = worker_task;

		//if (worker->isCarryingGas())
		//{
		//	worker->returnCargo();
		//}
		//else if (!worker->getPowerUp())
		//{

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

		//if (!worker->gather(worker->getClosestUnit(BWAPI::Filter::IsRefinery)))
		//{
		//	BWAPI::Broodwar << BWAPI::Broodwar->getLastError() << std::endl;
		//}
		//}
	}
}

void Worker::setWorkerTask(BWAPI::Unit worker, enum WorkerTask task)
{
	if (!worker)
		return;

	clearPreviousTask(worker);
	_worker_task_map[worker] = task;

	if (task == MINE)
	{
		
	}
	else if (task == BUILD)
	{

	}
	else if (task == GAS)
	{

	}
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

// TODO could optimize, instead of erase and insert, do copy/swap and insert
void Worker::clearPreviousTask(BWAPI::Unit unit)
{
	if (!unit)
		return;

	WorkerTask previous_task = getWorkerTask(unit);
	// TODO add MINE, etc.
	if (previous_task == MINE)
	{
		// ********** IN PROGRESS ************

		_depot_worker_count[_workers_depot_map[unit]] -= 1;
		_workers_depot_map.erase(unit);

		// TODO remove from mineral assignment
		// ********** IN PROGRESS ************
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

int Worker::getNumAssignedWorkers(BWAPI::Unit unit)
{
	if (!unit)
		return 0;

	std::map<BWAPI::Unit, int>::iterator it;

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
		// otherwise, set completed refineries
		else
		{
			_refinery_worker_count[unit] = 0;
		}
	}
}

// TODO rebalance function
void Worker::rebalanceWorkers()
{

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

/*
		// step 7: make pylons structure to keep track of their TilePosition, check if pylon exists 

		// step 8: getPositionBuilding(Building -> Pylon)

		// step 9: isBuildable() or canBuildHere()? 

		// step 10: verify minerals, etc. then constructor -> build(gateway, posBuildable)

		// step 11: send back to worker manager and set him to Mine Minerals enum?

		// step 12: create update() function to the constructor and worker manager

*/