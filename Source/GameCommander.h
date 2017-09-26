#pragma once

#include <BWAPI.h>
#include "ScoutManager.h"
#include "WorkerManager.h"
#include "DefenceManager.h"


namespace MagBot
{

	class GameCommander
	{
		BWAPI::Unitset	_valid_units;
		BWAPI::Unitset  _combat_units;
		BWAPI::Unitset  _scout_units;

		bool _initial_scout;
		bool _active_scout;

		void setScoutUnits();
		void setCombatUnits();

		void assignUnit(BWAPI::Unit unit, BWAPI::Unitset & set);
		bool isAssigned(BWAPI::Unit unit) const;

	public:
		GameCommander();
		~GameCommander();

		void update();

		BWAPI::Unit getFirstSupplyProvider();
		BWAPI::Unit getClosestWorkerToTarget(BWAPI::Position target);

	};

}