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
		BWAPI::Unit getBuilderClosestTo(BWAPI::TilePosition tile_position);
		bool build(BWAPI::UnitType unit_type);

		// TODO add handling for vespene gas max workers count (optimal 3)
		// TODO add handling for mineral max workers per mineral patch (optimal 1.5 to 2-3)

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