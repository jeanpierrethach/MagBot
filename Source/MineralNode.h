#pragma once

#include "Common.h"
#include <stdint.h>

namespace MagBot
{
	// change into class with worker and mineral patch as unit
	
	
	enum WorkerState {MOVING_TO_PATCH = 0, MINING = 1};
	
	class WorkerMining
	{

	public:
		//enum WorkerState { MOVING_TO_PATCH = 0, MINING = 1 };

		int frame_start_mining;
		int frame_start_moving;
		int worker_state;

		BWAPI::Unit mineral_patch;
		int mineral_patch_id;

		BWAPI::Unit worker;
		int worker_id;
		//uint8_t current_workers;
	
	public:
		WorkerMining(int frame_start_mining, int frame_start_moving,
			BWAPI::Unit mineral_patch, BWAPI::Unit worker)
		{
			frame_start_mining = frame_start_mining;
			frame_start_moving = frame_start_moving;
			mineral_patch = mineral_patch;
			worker = worker;
			mineral_patch_id = mineral_patch->getID();
			worker_id = worker->getID();
		};

		BWAPI::Unit getWorker() { return worker; }
		int getMineralPatchID() { return mineral_patch_id; }
		void setState(enum WorkerState state) { worker_state = state; }
	
	};

	class MineralNode
	{
		BWAPI::Unitset _mineralNodes;

		// int - mineral_patch_id
		std::map<int, std::deque<WorkerMining> > _mineral_patch;

	public:
		MineralNode();
		~MineralNode();

		std::map<int, std::deque<WorkerMining> > getMineralPatch();
		//int getCurrentMineral(int id);
		//uint8_t getCurrentWorkers(int id);

		void initializeMineralPatches();
		void insertMineralPatch(BWAPI::Unit unit);
		void removeMineralPatch(BWAPI::Unit unit);
		
		//bool hasWorker(WorkerMining worker, int patch_id);
		void insertWorkerToPatch(WorkerMining worker, int patch_id);// , std::map<int, std::deque<WorkerMining>> & mineral_nodes);
		void removeWorkerFromPatch(BWAPI::Unit unit);

		int getMineralIDFrom(BWAPI::Unit unit);

		BWAPI::Unitset getMineralNodes();
	};

}
