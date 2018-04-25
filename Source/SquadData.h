#pragma once

#include "Common.h"
#include "Unit.h"


namespace MagBot
{
	struct SquadComposition
	{
		BWAPI::Unit squad_leader = nullptr;
		std::map<BWAPI::Unit, Unit> squad_members;
	};

	class Squad
	{
		SquadComposition _squad;

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
				if (member.second.pos == pos)
				{
					return true;
				}
			}
			return false;
		}

		std::map<BWAPI::Unit, Unit> getMembers() const { return _squad.squad_members; }
		
		std::vector<Unit> units;

		void pushUnit(Unit unit) { units.push_back(unit); }
		const size_t sizeV() const { return units.size(); }

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

		std::deque<BWAPI::Unit> available_units;

		bool isEmpty();
		void addSquad(const Squad & squad);
		void findUnitandAssign(Squad & squad);
		void assignTo(Squad & squad);

		void add(BWAPI::Unit unit);
	};
}