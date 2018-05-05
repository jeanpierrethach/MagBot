#pragma once

#include "Common.h"
#include "SquadData.h"
#include <BWTA.h>
#include "Information/InformationManager.h"
#include "Strategery/Timers.h"
#include "Utilities/Utils.h"

namespace MagBot
{

	class SquadManager : Utils
	{
		SquadData _squads;

		SquadData _random_squads;
		void cohesionUpdate(const Squad & squad);
		void concaveSqUpdate(Squad & squad, std::vector<BWAPI::Position> v_target_location);

		void removeUnit(BWAPI::Unit unit);

	public:
		SquadManager();
		~SquadManager();

		bool assign = true;

		std::vector<BWAPI::Position> positions;

		virtual void smartAttackMove(BWAPI::Unit unit, const BWAPI::Position & target_location);

		void onUnitComplete(BWAPI::Unit unit);

		void update();	

		void onUnitDestroy(BWAPI::Unit unit);

		bool has_range_upgrade = false;

		static SquadManager & Instance();
	};
}