#pragma once

#include "Common.h"
#include "SquadData.h"
#include <BWTA.h>

namespace MagBot
{

	class SquadManager
	{
		//std::map<std::string, SquadData> _squads;
		SquadData _squads;
		void cohesionUpdate(const Squad & squad);
		void concaveSqUpdate(Squad & squad, std::vector<BWAPI::Position> v_target_location);
		void concaveUpdate(const Squad & squad, BWAPI::Position target_location);

		void dragoonKiteMicro(const Squad & squad);

		void smartAttackMove(BWAPI::Unit unit, const BWAPI::Position & target_location);

		void removeUnit(BWAPI::Unit unit);

		double radToDeg(double angle);
		double getAngleDeg(BWAPI::Position p1, BWAPI::Position p2);
		double getAngleRad(BWAPI::Position p1, BWAPI::Position p2);
		int getDistance(BWAPI::Position p1, BWAPI::Position p2);
		void average(BWAPI::Position & pos, int size);
		void normalize(BWAPI::Position & pos);

		// void rebalanceSquads();
		// void sendReinforcement(const Squad & squad);

	public:
		SquadManager();
		~SquadManager();

		void update();	

		void onUnitDestroy(BWAPI::Unit unit);

		static SquadManager & Instance();
	};
}

