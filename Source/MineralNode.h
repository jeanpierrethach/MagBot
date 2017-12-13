#pragma once

#include "Common.h"
#include <stdint.h>
#include <utility>
#include "WorkerNodeQueue.h"
#include "WorkerMining.h"
#include <algorithm>

namespace MagBot
{
	// change into class with worker and mineral patch as unit	
	
	

	class MineralNode
	{
		BWAPI::Unitset _mineralNodes;

		//std::vector<std::pair<int, std::deque<WorkerMining>> > _mineral_patch;

		

	public:
		MineralNode();
		~MineralNode();

		std::vector<WorkerNodeQueue> _deque_workers;

		void initializePatch();
		void insertWorkerToPatch(WorkerMining worker, int patch_id);
		void displayWorkerinDeque();

		WorkerMining & getWorkerMining(BWAPI::Unit unit);
		const WorkerMining & getWorkerMining(BWAPI::Unit unit) const;
		BWAPI::Unit getAssignedMineralPatch(BWAPI::Unit unit);

		void setFrameStartMining(BWAPI::Unit unit, int frame);
		const int getFrameStartMining(BWAPI::Unit unit);
		WorkerState getWorkerState(BWAPI::Unit unit);
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
