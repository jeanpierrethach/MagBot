#pragma once

#include <BWAPI.h>

namespace MagBot
{

	class ScoutManager
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