#include "SquadManager.h"

using namespace MagBot;

SquadManager::SquadManager()
{
	//Squad squad;
	//squad_data.assignUnitTo(squad);
	//squad_data.addSquad(squad);
}


SquadManager::~SquadManager()
{
}

void SquadManager::update()
{
	
	if (_squads.isEmpty())
	{
		BWAPI::Broodwar->sendText("_squads is empty");
		Squad squad;
		_squads.addSquad(squad);
	}

	// TODO if squad size > x then add new squad
	// TODO not apply to leader, leader moves

	for (auto & squad : _squads.getSquads())
	{
		_squads.findUnitandAssign(squad);
		if (squad.hasLeader())
		{
			BWAPI::Broodwar->drawCircleMap(squad.getLeader()->getPosition(), 20, BWAPI::Colors::Purple, true);
		}
		static int lastChecked {0};
		//static int lastChecked2 {0};

		// TODO if updating concave too fast, it cancels the attack animation command
		// could separate check if at good position (minimum of range attack distance)
		// then no need to send command to that unit
		// http://www.codeofhonor.com/blog/the-starcraft-path-finding-hack
		// https://www.youtube.com/watch?v=RzhTG-goqXc
		// https://github.com/TorchCraft/TorchCraft/issues/14
		// https://github.com/TorchCraft/TorchCraft/pull/102/files

		std::vector<BWAPI::Position> v;
		BWAPI::TilePosition base = BWAPI::Broodwar->self()->getStartLocation();
		BWTA::Chokepoint* cp = BWTA::getNearestChokepoint(BWAPI::Position(base));
		BWAPI::Position cp_center = cp->getCenter();
		v.push_back(cp_center);
		v.push_back(BWAPI::Position(cp_center.x, cp_center.y + 200));
		v.push_back(BWAPI::Position(cp_center.x, cp_center.y - 200));
		v.push_back(BWAPI::Position(cp_center.x + 200, cp_center.y + 200));
		concaveSqUpdate(squad, v);

		//concaveUpdate(squad, BWAPI::Position(3400, 1000));

		/*if (lastChecked2 + 90 < BWAPI::Broodwar->getFrameCount())
		{
			lastChecked2 = BWAPI::Broodwar->getFrameCount();
			cohesionUpdate(squad);
		}*/
		
		//dragoonKiteMicro(squad);
	}
}

// https://gamedevelopment.tutsplus.com/tutorials/3-simple-rules-of-flocking-behaviors-alignment-cohesion-and-separation--gamedev-3444
// http://harry.me/blog/2011/02/17/neat-algorithms-flocking/
// https://processing.org/examples/flocking.html

void SquadManager::cohesionUpdate(const Squad & squad)
{
	if (squad.size() <= 1)
		return;

	for (auto & member : squad.getMembers())
	{
		// doesn't seems to work? find a way to make entities of the flock follow leader
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
		smartAttackMove(member.first, center_of_mass); //member->attack(center_of_mass);

		// TODO replace center_of_mass with final_position
		//BWAPI::Broodwar->sendText("%d, %d", center_of_mass.x, center_of_mass.y);
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

// TODO add unit and target
void SquadManager::dragoonKiteMicro(const Squad & squad)
{
	BWAPI::Unit unit;

	if (!unit)
		return;

	const int ground_wep_cooldown = unit->getGroundWeaponCooldown();

	for (auto & member : squad.getMembers())
	{
		// TODO get angle and get opposite to move
		//if (member->isUnderAttack())
		//{
			BWAPI::Position pos = member.first->getPosition();
			
			//member->move()
		//}
	}

	// TODO if squad member is under attack and member is not then help
	// at the end of encounter regroup
}

// TODO fix BUG leader keeps showing debugging displays even when he's not moving??

// TODO squareSqUpdate
/*
create square vector of position with N positions (squad size), assign each member of squad to an available position
*/

// TODO sendOrderToMove to new target location
// to keep same formation, calculate new_x and new_y with current pos, current angle based on
// current pos and current target location and distance between current target location and new target location
// -- validate if new position for unit is valid (terrain), else attack to the new target location to find position
// -- could also check if the new position is occupied by a squad member


// DRAGOON PATHFINDING FLAWS
// https://day9.tv/d/Lasion/so-for-scbw-ai/
// add random location near target location? or send one member at different location near target location
// consider medium-sized unit (dragoon)
// position is subpixel? collision is pixel?

// member will be 'repulsed' from the closest target_location from its current position
// could also have vector (line of repulsion)

void SquadManager::concaveSqUpdate(Squad & squad, std::vector<BWAPI::Position> v_target_location)
{
	if (squad.size() <= 0)
		return;

	// TODO add countdown (timer on the repulsion) could put one for each target location?
	// with a map?

	// [FIXED] TODO FIX BUG, since it iterates from every member, if the current member isn't in range of the target_location,
	// then it will keep pointing to that member and won't try to update the other members of the squad
	// cases: that member could be the fisrt, second, etc...
	// SOL: return into continue 
	for (auto & member : squad.getMembers())
	{
		// TODO fix position is set, and add resetPosition in which case?
		if (member.second.isPositionSet())
		{
			BWAPI::Broodwar->sendText("%d, %d", member.second.getPosition().x, member.second.getPosition().y);
			continue;
		}

		BWAPI::Position pos = member.first->getPosition();
		double range = member.first->getType().groundWeapon().maxRange();

		
		if (BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Singularity_Charge) == 1)
		{
			range = 192; // Singularity increases attack range from 4 to 6. Times 32 pixels
		}

		double min_dist {9999};
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
		if (distance > range)
		{
			// TODO remove few steps (take in account the angle), can create a scale on repulsion?
			smartAttackMove(member.first, v_target_location[closest_index]); // member->attack(v_target_location[closest_index]);
			continue; // changed from return to continue
		}

		double step_distance = min_dist - range;
		double angle = getAngleDeg(pos, v_target_location[closest_index]);
		double new_pos_x = pos.x + (step_distance * cos(angle));
		double new_pos_y = pos.y - (step_distance * sin(angle));

		BWAPI::Position final_pos(new_pos_x, new_pos_y);

		if (squad.isPositionTaken(final_pos))
		{
			smartAttackMove(member.first, v_target_location[closest_index]);
			continue;
		}

		/*if (!BWAPI::Broodwar->isWalkable(BWAPI::WalkPosition(final_pos)))
		{
			smartAttackMove(member.first, v_target_location[closest_index]);
			BWAPI::Broodwar->sendText("Not walkable");
			continue;
		}*/

		member.second.setPosition(final_pos);
		//BWAPI::Broodwar->sendText("%s", member.second.isPositionSet() ? "true" : "false");
		smartAttackMove(member.first, final_pos); //member->attack(final_pos);

		//m_location.insert(std::pair<BWAPI::Unit, BWAPI::Position>(member, ));
		BWAPI::Broodwar->registerEvent([v_target_location, closest_index, range, member, pos](BWAPI::Game*)
		{
			for (auto & target_location : v_target_location)
			{
				BWAPI::Broodwar->drawCircleMap(target_location, 50, BWAPI::Colors::Orange, false);
			}
			BWAPI::Broodwar->drawLineMap(pos, member.first->getTargetPosition(), BWAPI::Colors::Cyan);
			BWAPI::Broodwar->drawCircleMap(member.first->getPosition(), 20, BWAPI::Colors::Red, false);
			BWAPI::Broodwar->drawTextMap(pos, "%d, %d", pos.x, pos.y);
			BWAPI::Broodwar->drawLineMap(pos, v_target_location[closest_index], BWAPI::Colors::White);		
			BWAPI::Broodwar->drawCircleMap(pos, range, BWAPI::Colors::Red, false);
		},   // action
			nullptr,    // condition
			30);

	}
}

void SquadManager::concaveUpdate(const Squad & squad, BWAPI::Position target_location)
{
	if (squad.size() <= 0)
		return;

	/*int count = squad.size();
	BWAPI::Position center_of_mass {0, 0};

	int separation_distance = 20;

	for (auto & member : squad.getMembers())
	{
		for (auto & other_member : squad.getMembers())
		{
			if (member != other_member)
			{
				BWAPI::Position position = other_member->getPosition();
				center_of_mass += position;
			}
		}

		average(center_of_mass, count);
	}*/

	for (auto & member : squad.getMembers())
	{
		BWAPI::Position pos = member.first->getPosition();
		double range = member.first->getType().groundWeapon().maxRange();
		double distance = member.first->getDistance(target_location); //getDistance(pos, target_location);

		if (distance > range)
		{
			// TODO remove few steps (take in account the angle), can create a scale on repulsion?
			smartAttackMove(member.first, target_location); //member->attack(target_location);
			return;
		}
		
		double step_distance = distance - range;
		const double PI = std::atan(1.0) * 4;
		double angle = getAngleDeg(pos, target_location);
		double new_pos_x = pos.x + (step_distance * cos(angle));
		double new_pos_y = pos.y - (step_distance * sin(angle));

		BWAPI::Position final_pos(new_pos_x, new_pos_y);
		smartAttackMove(member.first, final_pos);  //member->attack(final_pos);

		BWAPI::Broodwar->registerEvent([target_location, range, member, pos](BWAPI::Game*)
		{
			BWAPI::Broodwar->drawCircleMap(member.first->getPosition(), 20, BWAPI::Colors::Red, false);
			BWAPI::Broodwar->drawTextMap(pos, "%d, %d", pos.x, pos.y);
			BWAPI::Broodwar->drawLineMap(pos, target_location, BWAPI::Colors::White);
			BWAPI::Broodwar->drawCircleMap(target_location, 10, BWAPI::Colors::Orange, false);
			BWAPI::Broodwar->drawCircleMap(pos, range, BWAPI::Colors::Red, false);
		},   // action
			nullptr,    // condition
			60);
		
	}

}

// TODO move to micro

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

// TODO move to utils
double SquadManager::radToDeg(double angle)
{
	const double PI = std::atan(1.0) * 4;
	return angle * 180 / PI;
}

double SquadManager::getAngleDeg(BWAPI::Position p1, BWAPI::Position p2)
{
	const double PI = std::atan(1.0) * 4;
	return atan2(p1.y - p2.y, p1.x - p2.x) * 180 / PI;
}

double SquadManager::getAngleRad(BWAPI::Position p1, BWAPI::Position p2)
{
	return atan2(p1.y - p2.y, p1.x - p2.x);
}

int SquadManager::getDistance(BWAPI::Position p1, BWAPI::Position p2)
{
	return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

void SquadManager::average(BWAPI::Position & pos, int size)
{
	if (size != 0)
	{
		pos.x /= size;
		pos.y /= size;
	}
}

void SquadManager::normalize(BWAPI::Position & pos)
{
	double length = sqrt((pos.x * pos.x) + (pos.y * pos.y));
	if (length != 0)
	{
		pos.x = (pos.x / length);
		pos.y = (pos.y / length);
	}
}

// TODO implement flocking algorithm, implement concave formation (seek range/attack range from point of reference
// ref 
// http://gamedev.stackexchange.com/questions/55184/separation-algorithm-in-flock-simulation

/*

List of entities

Find neighbours

Add separation distance

Have a leader

Create a flock

Add leader and others in the flock

Update cohesion at certain interval of timeframes?

*/

/*
https://www.youtube.com/watch?v=yovUyQ9h81Q
cohesion (1,3) , (4,6) , (3,7) , (4,9)

find center of mass	of other entities (sum of position vector divided by count of entities)

for (1,3) -> sum (4,6) , (3,7) , (4,9) = (11,22) / 3 = 3.6, 7.3

then move that entity to the center of mass (find vector between source of entity and center of mass and normalize)

normalize -> u = v / |v|

for v = (3,4)
|v| = squared(pow(3) + pow(4))

u = (3,4) / 5

https://www.youtube.com/watch?v=rTMk0eVedOY


separation
https://www.youtube.com/watch?v=7HVVBONfLuE
https://www.youtube.com/watch?v=RpvjZNhEods

alignment
https://www.youtube.com/watch?v=EqChGlOKdKs
https://www.youtube.com/watch?v=ngixtDHt5jo

*/
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

SquadManager & SquadManager::Instance()
{
	static SquadManager instance;
	return instance;
}