#pragma once

#include "Common.h"

namespace MagBot
{
	// TODO:
	// check which enum needed
	enum WorkerState { MOVING_TO_PATCH = 0, MINING = 1, MOVING_TO_DEPOT = 2, NONE = 3 };

	class WorkerMining
	{
		
		//enum WorkerState { MOVING_TO_PATCH = 0, MINING = 1 };

		int frame_start_moving;
		
		
		int mineral_patch_id;

		
		int worker_id;
		//uint8_t current_workers;

	public:
		WorkerMining() {};
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
		~WorkerMining() {};

		WorkerState state;
		int frame_start_mining;
		BWAPI::Unit mineral_patch;
		BWAPI::Unit worker;

		int getWorkerID() { return worker_id; }
		int getMineralPatchID() { return mineral_patch_id; }
		//void setState(enum WorkerState state) { worker_state = state; }
		//WorkerState getState() { return worker_state; }
	};

}