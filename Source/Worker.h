#pragma once

#include "Common.h"
#include "MineralNode.h"
#include <stdint.h>

// quadtree

namespace MagBot
{
	class Worker
	{

	public:
		enum WorkerTask { MINE, GAS, BUILD, IDLE, SCOUT, DEFAULT };

	private:

		BWAPI::Unitset _workers;
		std::map<BWAPI::Unit, enum WorkerTask> _worker_task_map;
		std::map<BWAPI::Unit, BWAPI::UnitType> _worker_building_type_map;


		// TODO work on queue system + cooperative pathfinding (including collision detection)
		//std::map<BWAPI::Unit, BWAPI::Unit> _workers_mineral_assignment; // assign a worker for one mineral patch
		
		std::map<BWAPI::Unit, std::queue<BWAPI::Unit>> _worker_mineral_assignment;
		std::map<BWAPI::Unit, std::deque<BWAPI::Unit>> _mineral_workers_count; // mineral patch / deque

		std::map<BWAPI::Unit, BWAPI::Unit> _workers_depot_map; // worker / depot
		std::map<BWAPI::Unit, uint8_t> _depot_worker_count; // depot / number, retrieve count of workers at each depot

		std::map<BWAPI::Unit, BWAPI::Unit> _workers_refinery_map; // worker / refinery
		std::map<BWAPI::Unit, uint8_t> _refinery_worker_count; // refinery / number

		void clearPreviousTask(BWAPI::Unit unit);

		// TODO instantiate auto defense class object and in that class will have the micro aspect for workers

	public:
		
		Worker();
		~Worker();

		void update();

		void removeDestroyedWorker(BWAPI::Unit unit);

		enum WorkerTask getWorkerTask(BWAPI::Unit unit);
		BWAPI::UnitType getWorkerBuildingType(BWAPI::Unit unit);

		BWAPI::Unit getWorkerResource(BWAPI::Unit unit);

		uint8_t	getNumAssignedDepotWorkers(BWAPI::Unit unit);
		uint8_t getNumAssignedRefineryWorkers(BWAPI::Unit unit);
		
		void setWorkerTask(BWAPI::Unit worker, WorkerTask task, BWAPI::Unit worker_task);
		void setWorkerTask(BWAPI::Unit worker, WorkerTask task);
		void setWorkerTask(BWAPI::Unit worker, WorkerTask task, BWAPI::UnitType task_unit_type);

		const BWAPI::Unitset getWorkers() const { return _workers; }
	};

}