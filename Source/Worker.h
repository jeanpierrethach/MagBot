#pragma once

#include "Common.h"


namespace MagBot
{
	//TODO add worker stats

	class Worker
	{

	public:
		enum WorkerTask { Mine, Gas, Build, Idle, Default };

	private:

		// could change so i can retrieve a specific worker?
		BWAPI::Unitset _workers;
		std::map<BWAPI::Unit, enum WorkerTask> _workerTaskMap;
		std::map<BWAPI::Unit, BWAPI::UnitType> _workerBuildingTypeMap;

		void clearPreviousTask(BWAPI::Unit unit);

	public:
		
		Worker();
		~Worker();

		// TODO could add all current workers with Build task, Mine task, etc.. so no need to iterate over all workers
		
		// void addWorker
		void update();

		enum WorkerTask getWorkerTask(BWAPI::Unit unit);
		BWAPI::UnitType getWorkerBuildingType(BWAPI::Unit unit);
		
		void setWorkerTask(BWAPI::Unit unit, WorkerTask task);
		void setWorkerTask(BWAPI::Unit worker, WorkerTask task, BWAPI::UnitType taskUnitType);

		const BWAPI::Unitset getWorkers() const { return _workers; }
	};

}