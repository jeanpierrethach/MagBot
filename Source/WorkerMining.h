#pragma once

#include "Common.h"

namespace MagBot
{

	enum WorkerState { MOVING_TO_PATCH = 0, MINING = 1, MOVING_TO_DEPOT = 2, NONE = 3, SCOUT = 4 };

	class WorkerMining
	{
	public:
		WorkerMining(int frame_start_mining, int frame_start_moving,
			BWAPI::Unit mineral_patch, BWAPI::Unit worker) : frame_start_mining(frame_start_mining),
			frame_start_moving(frame_start_moving), mineral_patch(mineral_patch), worker(worker)
		{
			mineral_patch_id = mineral_patch->getID();
			worker_id = worker->getID();
			patch_pos_x = mineral_patch->getPosition().x;
			patch_pos_y = mineral_patch->getPosition().y;
		};

		WorkerState state;
		int frame_start_moving;
		int frame_start_mining;
		BWAPI::Unit mineral_patch;
		BWAPI::Unit worker;

		int worker_id;
		int mineral_patch_id;
		int patch_pos_x;
		int patch_pos_y;
	};

}