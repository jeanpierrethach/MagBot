#pragma once

#include "Common.h"
#include "Unit.h"

namespace MagBot
{
	struct SquadComposition
	{
		BWAPI::Unit squad_leader = nullptr;
		std::map<BWAPI::Unit, Unit> squad_members;
		//BWAPI::Unitset squad_members;
	};

	class Squad
	{
		SquadComposition _squad;

		//int _separation_distance;
		//int _overall_health;
		//int _overall_power;

	public:
		BWAPI::Unit getLeader() const { return _squad.squad_leader; }
		void setLeader(BWAPI::Unit unit) { _squad.squad_leader = unit; }
		bool hasLeader() { return _squad.squad_leader != nullptr; }
		bool containsUnit(BWAPI::Unit unit)
		{
			const std::map<BWAPI::Unit, Unit>::const_iterator it = _squad.squad_members.find(unit);

			if (it != _squad.squad_members.end())
			{
				return true;
			}
			return false;
		}

		bool isPositionTaken(BWAPI::Position pos)
		{
			for (auto & member : _squad.squad_members)
			{
				if (member.second.getPosition() == pos)
				{
					return true;
				}
			}
			return false;
		}

		std::map<BWAPI::Unit, Unit> getMembers() const { return _squad.squad_members; }

		void addUnit(BWAPI::Unit unit, Unit u) { _squad.squad_members[unit] = u; }
		void removeUnit(BWAPI::Unit unit) { if (!unit) return;  _squad.squad_members.erase(unit); }
		const size_t size() const { return _squad.squad_members.size(); }
	};

	class SquadData
	{
		std::vector<Squad> _squads;

	public:
		SquadData();
		~SquadData();

		std::vector<Squad> getSquads() const { return _squads; }

		//void updateSquads();

		bool isEmpty();
		void addSquad(const Squad & squad);
		//void assignUnitTo(); // BWAPI::Unit unit,  const Squad & squad
		void findUnitandAssign(Squad & squad);

		//Squad findNearestSquad(const Squad & squad);
	};
}