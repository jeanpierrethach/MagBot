#pragma once

#include <BWAPI.h>
#include "Worker.h"

namespace MagBot {

	class WorkerManager
	{
		Worker _worker;

	public:
		WorkerManager();
		~WorkerManager();

		void update();
		void updateWorkerTask();
		

		BWAPI::Unit getBuilder();
		void build(BWAPI::UnitType unitType);


		void showDebugWorkerInfo(BWAPI::Unit unit);

		static WorkerManager & Instance();

		//void addWorker();
		//void removeWorker()
		//void freeWorker()
		
		//bool exists()

		//void finishedWithWorker(BWAPI::Unit unit);
		//bool isWorkerScout(BWAPI::Unit worker);

		void handleMineralWorkers(BWAPI::Unit unit);
		void handleIdleWorkers(BWAPI::Unit unit);
	};

}