#pragma once

#include <BWAPI.h>
#include "Worker.h"
#include "BuildingManager.h"
#include "Information/InformationManager.h"
#include "MineralNode.h"
#include <stdint.h>
#include <deque>

#include <thread>
#include <future>
#include <string>
#include <chrono>
#include <mutex>
#include <condition_variable>

namespace MagBot {

	class WorkerManager
	{
		Worker _worker;
		MineralNode _mineral_nodes;

		void handleMineralWorkers();
		void handleGasWorkers();
		void handleIdleWorkers();

		std::pair<int, int> getRangePair(int no_future, int size, int nb_threads);

		std::pair<BWAPI::Unit, int> calculateBestPatch(BWAPI::Unit worker, const int begin, const int end);
		void assignBestPatch(BWAPI::Unit worker);
		
	public:
		WorkerManager();
		~WorkerManager();

		void update();
		void updateWorkerCount();
		
		BWAPI::Unit getBuilder(Building & building);
		BWAPI::Unit getClosestDepot(BWAPI::Unit worker);
		BWAPI::Unit getGasWorker(BWAPI::Unit refinery);

		void optimizeWorkersMining();

		void setWorkerScout(BWAPI::Unit worker);

		void showDebugWorkerInfo(const BWAPI::Unit & worker);

		void onUnitDestroy(BWAPI::Unit unit);

		static WorkerManager & Instance();

		std::string getWorkerTaskName(BWAPI::Unit worker);

		void setWorkerFree(BWAPI::Unit worker);
	};

}