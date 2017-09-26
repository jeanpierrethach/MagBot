#include "Common.h"
#include "SquadManager.h"

#pragma once

namespace MagBot
{

	class DefenceManager : SquadManager
	{

	public : 
		enum WorkerTask { IDLE, COMBAT };
		
	private :
		BWAPI::Unitset _workers;
		std::map<BWAPI::Unit, WorkerTask> _worker_task;
		std::vector<int> _zones_radius;

		std::map<Squad, BWAPI::Position> _units_in_combat;

	public:

		DefenceManager();
		~DefenceManager();

		void setCombatUnits();
		void assignWorkersTo(BWAPI::Unit unit, WorkerTask task, BWAPI::Position position);

		static DefenceManager & Instance();
	};


}