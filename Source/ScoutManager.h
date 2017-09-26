#pragma once

#include <BWAPI.h>
#include "WorkerManager.h"
#include <BWTA.h>
#include "InformationManager.h"

namespace MagBot
{

	class ScoutManager : InformationManager
	{

		BWAPI::Unit	_worker_scout;

	public:
		ScoutManager();
		~ScoutManager();

		void update();

		void setWorkerScout(BWAPI::Unit unit);

		static ScoutManager & Instance();
	};

}