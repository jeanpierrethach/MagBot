#pragma once

#include <BWAPI.h>
#include "Worker.h"
#include "BuildingManager.h"
#include <stdint.h>

namespace MagBot {

	class WorkerManager
	{
		Worker _worker;

		void handleMineralWorkers();
		void handleGasWorkers();
		void handleIdleWorkers();

	public:
		WorkerManager();
		~WorkerManager();

		void update();
		void updateWorkerCount();
		
		BWAPI::Unit getBuilder(Building & building);
		BWAPI::Unit getBuilderClosestTo(BWAPI::TilePosition tile_position);
		BWAPI::Unit getClosestDepot(BWAPI::Unit worker);

		BWAPI::Unit getGasWorker(BWAPI::Unit refinery);

		// TODO add handling for vespene gas max workers count (optimal 3)
		// TODO add handling for mineral max workers per mineral patch (optimal 1.5 to 2-3)

		void setWorkerScout(BWAPI::Unit worker);

		void showDebugWorkerInfo(const BWAPI::Unit & worker);

		static WorkerManager & Instance();

		std::string getWorkerTaskName(BWAPI::Unit worker);

		//void addWorker();
		//void removeWorker()
		void setWorkerFree(BWAPI::Unit worker);
		
		//bool exists()

		//bool isWorkerScout(BWAPI::Unit worker);
	};

}