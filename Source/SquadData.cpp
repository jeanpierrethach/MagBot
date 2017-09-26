#include "SquadData.h"

using namespace MagBot;

SquadData::SquadData()
{
}


SquadData::~SquadData()
{
}

bool SquadData::isEmpty()
{
	return _squads.empty();
}

void SquadData::addSquad(const Squad & squad)
{
	_squads.push_back(squad);
}


void SquadData::findUnitandAssign(Squad & squad)
{
	for (const auto & unit : BWAPI::Broodwar->self()->getUnits())
	{
		if (!unit->exists() || !unit->isCompleted())
			continue;

		BWAPI::UnitType dragoon = BWAPI::UnitTypes::Protoss_Dragoon;

		if (unit->getType() == dragoon && !squad.containsUnit(unit))
		{
			if (!squad.hasLeader())
			{
				squad.setLeader(unit);
			}
			Unit u;
			squad.addUnit(unit, u);
		}
	}
}