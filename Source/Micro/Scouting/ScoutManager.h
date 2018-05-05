#pragma once

#include <BWAPI.h>
#include "Macro/WorkerManager.h"
#include <BWTA.h>
#include "Information/InformationManager.h"
#include "Micro/Scouting/Scout.h"

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