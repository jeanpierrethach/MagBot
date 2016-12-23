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

		BWAPI::Unitset _workers;
		std::map<BWAPI::Unit, enum WorkerTask> _workerTaskMap;
		std::map<BWAPI::Unit, BWAPI::UnitType> _workerBuildingTypeMap;

		void clearPreviousTask(BWAPI::Unit unit);

	public:
		
		Worker();
		~Worker();
		
		// void addWorker
		void update();

		enum WorkerTask getWorkerTask(BWAPI::Unit unit);
		
		void setWorkerTask(BWAPI::Unit unit, WorkerTask task);
		void setWorkerTask(BWAPI::Unit worker, WorkerTask task, BWAPI::UnitType taskUnitType);

		const BWAPI::Unitset getWorkers() const { return _workers; }
	};

}