#pragma once

#include <BWAPI.h>

namespace MagBot
{

	class ScoutManager
	{

		BWAPI::Unit	_workerScout;


	public:
		ScoutManager();
		~ScoutManager();

		void update();

		void setWorkerScout(BWAPI::Unit unit);

		static ScoutManager & Instance();
	};

}