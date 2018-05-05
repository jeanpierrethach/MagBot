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
	while (!available_units.empty())
	{
		BWAPI::Unit unit = available_units.front();
		if (!unit->exists())
			continue;

		Unit u(unit);
		squad.pushUnit(u);
		available_units.pop_front();
	
	}
}

void SquadData::assignTo(Squad & squad)
{
	BWAPI::UnitType drag = BWAPI::UnitTypes::Protoss_Dragoon;
	for (auto & unit : BWAPI::Broodwar->self()->getUnits())
	{
		if (unit->getType() == drag)
		{
			if (!unit->exists())
				continue;
			if (squad.containsUnit(unit))
				continue;
			if (!squad.hasLeader())
			{
				Unit u = Unit(unit);
				squad.addUnit(unit, u);
			}
		}
	}
}

void SquadData::add(BWAPI::Unit unit)
{
	BWAPI::UnitType dragoon = BWAPI::UnitTypes::Protoss_Dragoon;

	if (unit->getType() == dragoon)
	{
		available_units.push_back(unit);
	}
}