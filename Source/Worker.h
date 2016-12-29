#pragma once

#include "Common.h"


namespace MagBot
{
	//TODO add worker stats

	class Worker
	{

	public:
		enum WorkerTask { MINE, GAS, BUILD, IDLE, DEFAULT };

	private:

		// could change so i can retrieve a specific worker?
		BWAPI::Unitset _workers;
		std::map<BWAPI::Unit, enum WorkerTask> _worker_task_map;
		std::map<BWAPI::Unit, BWAPI::UnitType> _worker_building_type_map;

		void clearPreviousTask(BWAPI::Unit unit);

		// TODO instantiate auto defense class object and in that class will have the micro aspect for workers

	public:
		
		Worker();
		~Worker();

		// TODO could add all current workers with Build task, Mine task, etc.. so no need to iterate over all workers
		
		// void addWorker
		void update();

		void removeDestroyedWorker();

		enum WorkerTask getWorkerTask(BWAPI::Unit unit);
		BWAPI::UnitType getWorkerBuildingType(BWAPI::Unit unit);
		
		void setWorkerTask(BWAPI::Unit unit, WorkerTask task);
		void setWorkerTask(BWAPI::Unit worker, WorkerTask task, BWAPI::UnitType task_unit_type);

		const BWAPI::Unitset getWorkers() const { return _workers; }
	};

}