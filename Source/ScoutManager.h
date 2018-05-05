#pragma once

#include <BWAPI.h>
#include "WorkerManager.h"
#include <BWTA.h>
#include "InformationManager.h"
#include "Scout.h"

namespace MagBot
{

	class ScoutManager
	{
		Scout _worker_scout;
		bool _enemy_base_location = false;

	public:
		ScoutManager();
		~ScoutManager();

		void update();

		void setWorkerScout(BWAPI::Unit unit);

		static ScoutManager & Instance();
	};

}