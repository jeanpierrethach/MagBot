#pragma once

//#include "Common.h"
#include <BWAPI.h>
#include <deque>
#include "WorkerMining.h"

namespace MagBot
{

	class WorkerNodeQueue
	{

	public:
		WorkerNodeQueue(BWAPI::Unit p, int id)
			: patch(p), patch_id(id)
		{
		}

		//BWAPI::Unitset minerals;

		// TODO:
		// MAKE IT AS A STRUCT instead?

		std::deque<WorkerMining> deque;
		BWAPI::Unit patch;
		const int patch_id;

		//std::deque<WorkerMining> getDeque() { return deque; }
		//void setPatchId(int id) { patch_id = id; }
		//int getPatchID() { return patch_id; }

		//void insertMineralPatch(BWAPI::Unit unit);
		
		
		
		
	};

}
