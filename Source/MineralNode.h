#pragma once

#include "Common.h"
#include <stdint.h>
#include <utility>
#include "WorkerNodeQueue.h"
#include "WorkerMining.h"
#include <algorithm>



#include <mutex>

namespace MagBot
{
	// change into class with worker and mineral patch as unit	
	
	struct WorkerNodeDeque
	{
		std::deque<WorkerMining> deque;
		BWAPI::Unit patch;
		int patch_id;
	};

	class MineralNode
	{
		BWAPI::Unitset _mineralNodes;

		//std::vector<std::pair<int, std::deque<WorkerMining>> > _mineral_patch;

		

	public:
		MineralNode();

		// TODO add mutex per deque
		// TODO how to get _deque_workers updated if thread has modified
		// during another thread
		
		//std::vector<WorkerNodeQueue> _deque_workers;

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



		// ----------------------------------

		/*std::vector<std::pair<int, std::deque<WorkerMining>> > getMineralPatch();
		
		void initializeMineralPatches();
		
		void removeMineralPatch(BWAPI::Unit unit);
		
		//bool hasWorker(WorkerMining worker, int patch_id);
		void insertWorkerToPatch(WorkerMining worker, int patch_id);// , std::map<int, std::deque<WorkerMining>> & mineral_nodes);
		void removeWorkerFromPatch(BWAPI::Unit unit);

		int getMineralIDFrom(BWAPI::Unit unit);
		*/
		BWAPI::Unitset getMineralNodes();
	};


	// WIP
	/*class Test
	{

	public:

		std::vector<DequeWorker> deque_vec;

	};*/

}
