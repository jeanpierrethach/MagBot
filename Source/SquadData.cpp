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

// void SquadData::assignUnitTo() // BWAPI::Unit unit, const Squad & squad

void SquadData::findUnitandAssign(Squad & squad) // BWAPI::Unit unit, const Squad & squad
{
	// TODO if squad size > x then return;

	for (const auto & unit : BWAPI::Broodwar->self()->getUnits())
	{
		if (!unit->exists() || !unit->isCompleted())
			continue;

		BWAPI::UnitType dragoon = BWAPI::UnitTypes::Protoss_Dragoon;

		if (unit->getType() == dragoon && !squad.containsUnit(unit))
		{
			if (!squad.hasLeader())
				// TODO fix setLeader, it is setting a new one?
				squad.setLeader(unit);
			Unit u;
			squad.addUnit(unit, u);
		}
	}
}