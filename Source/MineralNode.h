#pragma once

#include "Common.h"
#include <stdint.h>
#include <utility>
#include "WorkerMining.h"
#include <algorithm>



#include <mutex>

namespace MagBot
{
	struct WorkerNodeDeque
	{
		std::deque<WorkerMining> deque;
		BWAPI::Unit patch;
		int patch_id;
	};

	class MineralNode
	{
		BWAPI::Unitset _mineralNodes;

	public:
		MineralNode();

		std::vector<WorkerNodeDeque> _deque_workers;
		std::deque<std::mutex> mutexes;

		void initializePatch();

		void insertWorkerToPatch(WorkerMining worker, int patch_id);
		void insertWorkerToPatchParallel(WorkerMining worker, int patch_id);
		
		void displayWorkerinDeque();

		WorkerMining & getWorkerMining(BWAPI::Unit unit);
		const WorkerMining & getWorkerMining(BWAPI::Unit unit) const;
		const BWAPI::Unit getAssignedMineralPatch(BWAPI::Unit unit) const;

		void setFrameStartMining(BWAPI::Unit unit, int frame);
		const int getFrameStartMining(BWAPI::Unit unit);
		const WorkerState getWorkerState(BWAPI::Unit unit) const;
		void setWorkerState(BWAPI::Unit unit, WorkerState state);

		void removeWorkerFromPatch(BWAPI::Unit unit);

		void insertMineralPatch(BWAPI::Unit unit);

		BWAPI::Unitset getMineralNodes();
	};
}
