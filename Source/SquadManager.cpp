#include "SquadManager.h"

using namespace MagBot;

SquadManager::SquadManager()
{
	BWAPI::TilePosition base = BWAPI::Broodwar->self()->getStartLocation();
	BWTA::Chokepoint* cp = BWTA::getNearestChokepoint(BWAPI::Position(base));
	BWAPI::Position cp_center = cp->getCenter();
	positions.push_back(cp_center);
	positions.push_back(BWAPI::Position(cp_center.x - 100, cp_center.y - 200));
	positions.push_back(BWAPI::Position(cp_center.x - 200, cp_center.y - 200));
	positions.push_back(BWAPI::Position(cp_center.x, cp_center.y + 200));
	positions.push_back(BWAPI::Position(cp_center.x, cp_center.y - 200));
	positions.push_back(BWAPI::Position(cp_center.x, cp_center.y - 100));
	positions.push_back(BWAPI::Position(cp_center.x, cp_center.y + 100));
	positions.push_back(BWAPI::Position(cp_center.x + 100, cp_center.y + 200));
	positions.push_back(BWAPI::Position(cp_center.x + 200, cp_center.y + 200));

}


SquadManager::~SquadManager()
{
}

void SquadManager::update()
{
	
	if (_squads.isEmpty())
	{
		Squad squad;
		_squads.addSquad(squad);
	}

	for (auto & squad : _squads.getSquads())
	{
		if (assign)
		{
			_squads.assignTo(squad);
		}
		
		if (squad.hasLeader())
		{
			BWAPI::Broodwar->drawCircleMap(squad.getLeader()->getPosition(), 20, BWAPI::Colors::Purple, true);
		}

		if (BWAPI::Broodwar->getFrameCount() < TrainingWeights::AttackTimers::StartInterval)
		{
			concaveSqUpdate(squad, positions);
		}
		else if (BWAPI::Broodwar->getFrameCount() >= TrainingWeights::AttackTimers::StartInterval 
			&& BWAPI::Broodwar->getFrameCount() < TrainingWeights::AttackTimers::EndInterval)
		{
			for (auto & member : squad.getMembers())
			{
				BWAPI::Position enemy_base_location = InformationManager::Instance().getEnemyBaseLocation();
				BWTA::Chokepoint* enemy_cp = BWTA::getNearestChokepoint(enemy_base_location);
				smartAttackMove(member.first, enemy_cp->getCenter());
			}
		}
		// TODO attack in random locations when all enemy buildings discovered are destroyed
		else if (BWAPI::Broodwar->getFrameCount() >= 20000)
		{
			assign = false;
			for (auto & member : squad.getMembers())
			{
				Squad new_squad;

				new_squad.addUnit(member.first, member.second);
				squad.removeUnit(member.first);
				_random_squads.addSquad(squad);
			}
		}
		else
		{
			for (auto & member : squad.getMembers())
			{
				smartAttackMove(member.first, InformationManager::Instance().getEnemyBaseLocation());
			}
		}
		
		int count {0};
		for (auto & member : squad.units)
		{
			BWAPI::Broodwar->drawTextScreen(360, 100 + count, "%d, %d", member.unit->getPosition().x, member.unit->getPosition().y);
			count += 10;
		}
	}



	int map_width = BWAPI::Broodwar->mapWidth();
	int map_height = BWAPI::Broodwar->mapHeight();
	

	std::srand(std::time(nullptr));

	for (auto & squad : _random_squads.getSquads())
	{
		for (auto & member : squad.getMembers())
		{
			if (member.second.target_pos != BWAPI::Positions::None)
			{
				continue;
			}

			int rnd_x = std::rand() % map_width;
			int rnd_y = std::rand() % map_height;

			BWAPI::TilePosition rnd_tile = BWAPI::TilePosition(rnd_x, rnd_y);
			BWAPI::Position target_pos = BWAPI::Position(rnd_x / TILE_SIZE, rnd_y / TILE_SIZE);
			
			member.second.target_pos = target_pos;
			smartAttackMove(member.first, target_pos);
			
		}
	}

}

void SquadManager::cohesionUpdate(const Squad & squad)
{
	if (squad.size() <= 1)
		return;

	for (auto & member : squad.getMembers())
	{
		//if (member == squad.getLeader())
		//	continue;
		BWAPI::Position pos = member.first->getPosition();
		BWAPI::Position center_of_mass {0, 0};
		BWAPI::Position final_position {0, 0};

		for (auto & other_member : squad.getMembers())
		{
			if (member != other_member)
			{
				BWAPI::Position position = other_member.first->getPosition();
				center_of_mass += position;
			}
		}
		// TODO add max distance or weight on distance from flock and entity so that the center of mass doesn't relocate backwards
		// when adding new entity

		average(center_of_mass, squad.getMembers().size() - 1);

		//final_position = center_of_mass - pos;
		//normalize(final_position);
		//member->attack(final_position);
		smartAttackMove(member.first, center_of_mass);

		BWAPI::Broodwar->registerEvent([center_of_mass, member, pos](BWAPI::Game*)
		{
			BWAPI::Broodwar->drawCircleMap(member.first->getPosition(), 20, BWAPI::Colors::Red, false);
			BWAPI::Broodwar->drawTextMap(pos, "%d, %d", pos.x, pos.y);
			BWAPI::Broodwar->drawLineMap(pos, center_of_mass, BWAPI::Colors::White);
			BWAPI::Broodwar->drawCircleMap(center_of_mass, 10, BWAPI::Colors::Orange, false);
		},   // action
			nullptr,    // condition
			60);
		

		
	}
}

void SquadManager::concaveSqUpdate(Squad & squad, std::vector<BWAPI::Position> v_target_location)
{
	if (squad.size() <= 0)
		return;

	for (auto & member : squad.getMembers())
	{
		BWAPI::Broodwar->registerEvent([member](BWAPI::Game*)
		{
			BWAPI::Position pos = member.first->getPosition();
			BWAPI::Broodwar->drawTextMap(pos, "%d, %d", member.second.has_sing_charge, member.second.has_position);
		},   // action
			nullptr,    // condition
			30);

		if (!has_range_upgrade)
		{
			member.second.range = member.first->getType().groundWeapon().maxRange();
		}
		else
		{
			member.second.range = 192; // Singularity increases attack range from 4 to 6. Times 32 pixels
		}
		
		// TODO fix: seems has_sing_charge stays false
		if (!has_range_upgrade && BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Singularity_Charge) == 1)
		{
			member.second.resetPosition();
			has_range_upgrade = true;
		}

		if (member.second.has_position && member.second.pos != BWAPI::Positions::None)
		{
			smartAttackMove(member.first, member.second.pos);
			BWAPI::Broodwar->drawCircleMap(member.second.pos, 15, BWAPI::Colors::Teal, true);
			continue;
		}

		BWAPI::Position pos = member.first->getPosition();

		double min_dist = std::numeric_limits<double>::max();
		int closest_index {0};

		for (size_t i {0}; i < v_target_location.size(); ++i)
		{
			double distance = member.first->getDistance(v_target_location[i]);
			
			if (distance < min_dist)
			{
				min_dist = distance;
				closest_index = i;
			}
		}

		double distance = member.first->getDistance(v_target_location[closest_index]);

		if (distance > member.second.range + 20)
		{
			smartAttackMove(member.first, v_target_location[closest_index]);
			continue;
		}

		double step_distance = min_dist - member.second.range;
		double angle = getAngleDeg(pos, v_target_location[closest_index]);
		
		double new_pos_x = pos.x + (step_distance * cos(angle));
		double new_pos_y = pos.y - (step_distance * sin(angle));

		BWAPI::Position final_pos(new_pos_x, new_pos_y);

		if (!final_pos.isValid())
		{
			continue;
		}

		// TODO: Find valid position, reserve and assign to unit, move to target location
		// may be caused by this -> position is set, then it iterates through the position taken, it is taken,
		// so it recalculate a new position for the current member
		if (squad.isPositionTaken(final_pos))
		{
			smartAttackMove(member.first, v_target_location[closest_index]);
			continue;
		}
	
		smartAttackMove(member.first, final_pos);
		member.second.pos = final_pos;
		member.second.has_position = true;
		
		BWAPI::Broodwar->drawCircleMap(final_pos, 15, BWAPI::Colors::Green, true);

		BWAPI::Broodwar->registerEvent([v_target_location, closest_index, member, pos](BWAPI::Game*)
		{
			for (auto & target_location : v_target_location)
			{
				BWAPI::Broodwar->drawCircleMap(target_location, 50, BWAPI::Colors::Orange, false);
			}
			BWAPI::Broodwar->drawLineMap(pos, member.first->getTargetPosition(), BWAPI::Colors::Cyan);
			BWAPI::Broodwar->drawCircleMap(member.first->getPosition(), 20, BWAPI::Colors::Red, false);
			//BWAPI::Broodwar->drawTextMap(pos, "%d, %d", pos.x, pos.y);
			BWAPI::Broodwar->drawLineMap(pos, v_target_location[closest_index], BWAPI::Colors::White);		
			BWAPI::Broodwar->drawCircleMap(pos, member.second.range, BWAPI::Colors::Red, false);
		},   // action
			nullptr,    // condition
			30);
	}
}

void SquadManager::smartAttackMove(BWAPI::Unit unit, const BWAPI::Position & target_location)
{
	if (!unit || !target_location.isValid())
	{
		return;
	}

	// if we have issued a command to this unit already this frame, ignore this one
	if (unit->getLastCommandFrame() >= BWAPI::Broodwar->getFrameCount() || unit->isAttackFrame())
	{
		return;
	}

	// get the unit's current command
	BWAPI::UnitCommand currentCommand(unit->getLastCommand());

	// if we've already told this unit to attack this target, ignore this command
	if (currentCommand.getType() == BWAPI::UnitCommandTypes::Attack_Move &&	currentCommand.getTargetPosition() == target_location)
	{
		return;
	}

	// if nothing prevents it, attack the target
	unit->attack(target_location);
}

void SquadManager::removeUnit(BWAPI::Unit unit)
{
	for (auto & squad : _squads.getSquads())
	{
		if (squad.containsUnit(unit))
		{
			squad.removeUnit(unit);
		}
	}
}

void SquadManager::onUnitDestroy(BWAPI::Unit unit)
{
	if (!unit)
		return;

	if (unit->getPlayer() == BWAPI::Broodwar->self())
	{
		removeUnit(unit);
	}
}

void SquadManager::onUnitComplete(BWAPI::Unit unit)
{
	if (!unit)
		return;

	if (unit->getPlayer() == BWAPI::Broodwar->self())
	{
		_squads.add(unit);
	}	
}

SquadManager & SquadManager::Instance()
{
	static SquadManager instance;
	return instance;
}