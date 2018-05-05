#include "Scout.h"

using namespace MagBot;

Scout::Scout()
{
	enemy_base_location = InformationManager::Instance().getEnemyBaseLocation();
}


Scout::~Scout()
{
}

std::vector<Actions> Scout::getActions(State state)
{
	std::vector<Actions> actions;

	switch (state)
	{
		case State::MOVING:
			return actions;
		case State::CIRCLING:
			return actions;
		case State::ATTACKING:
			return actions;
		case State::IDLE:
		default:
			return actions;
	}
	return actions;
}

int Scout::heuristicCost(State state, Goal goal)
{
	return 0;
}

int Scout::actionCost(Actions actions)
{
	return 0;
}

Actions Scout::selectAction(State state)
{
	return Actions::NONE;
}

bool Scout::testGoal(State state)
{ 
	return state == goal;
}

void Scout::updateLocation()
{
	// TODO function that generates tiles in a matrix data structure and draws it
	int vision = scout->getType().sightRange();

	int scout_pos_x = scout->getPosition().x;
	int scout_pos_y = scout->getPosition().y;

	BWAPI::Broodwar->drawBoxMap(scout_pos_x - vision, scout_pos_y - vision, 
		scout_pos_x + vision, scout_pos_y + vision, BWAPI::Colors::Red, false);

	int start_vision_x_l = scout_pos_x - vision;
	int start_vision_y_t = scout_pos_y - vision;
	int start_vision_y_b = scout_pos_y + vision;
	int start_vision_x_r = scout_pos_x + vision;

	for (int i = 0; i <= 2 * vision / TILE_SIZE; ++i)
	{
		BWAPI::Broodwar->drawLineMap(start_vision_x_l + (i * TILE_SIZE), start_vision_y_t,
			start_vision_x_l + (i * TILE_SIZE), start_vision_y_b, BWAPI::Colors::Red);

		BWAPI::Broodwar->drawLineMap(start_vision_x_l, start_vision_y_t + (i * TILE_SIZE),
			start_vision_x_r, start_vision_y_t + (i * TILE_SIZE), BWAPI::Colors::Red);
	}
	
	// TODO use manhattan distance to evaluate best tiles
	// related to the goal; which is to create distance between
	// enemy units and scout allowing him to scout more/live longer

	// TODO detect enemy buildings
	// either going for each tiles of the scout or going with visibility of the player
	// maybe apply for every unit we own instead of only scout
	// ISSUE: pixel are detected on the screen and on the map, however the tileposition isn't

	BWAPI::Unitset units = BWAPI::Broodwar->getUnitsInRadius(scout->getPosition(), vision);

	for (auto enemy_unit : units)
	{
		if (enemy_unit->getPlayer() == BWAPI::Broodwar->enemy())
		{
			if (!InformationManager::Instance().getEnemyUnits().contains(enemy_unit))
			{
				InformationManager::Instance().insertEnemyUnit(enemy_unit);
			}
		}
	}

	// Update tileposition of enemy units
	for (auto unit : InformationManager::Instance().getEnemyUnits())
	{
		BWAPI::Broodwar->drawBoxMap(BWAPI::Position(unit->getTilePosition()),
			BWAPI::Position(unit->getTilePosition() + unit->getType().tileSize()), BWAPI::Colors::Cyan, false);	
	}

	// TODO update count of enemy units
	// display count on the side
}

void Scout::scoutBaseLocations()
{
	//int scout_hp = _worker_scout.scout->getHitPoints() + _worker_scout.scout->getShields();

	//BWTA::BaseLocation * enemy_base_location = nullptr;

	if (!_enemy_base_location)
	{
		for (BWTA::BaseLocation * start_location : BWTA::getStartLocations())
		{
			BWAPI::TilePosition start_location_tilepos = start_location->getTilePosition();
			InformationManager::Instance().setStartingBaseLocation(BWAPI::Position(start_location->getPosition()));

			if (BWAPI::Broodwar->isVisible(start_location_tilepos))
			{
				for (auto unit : BWAPI::Broodwar->enemy()->getUnits())
				{
					if (unit->getType().isResourceDepot() && unit->getTilePosition() == start_location_tilepos)
					{
						BWAPI::Broodwar->sendText("Found enemy base location based on tile position");
						InformationManager::Instance().setEnemyStartingBaseLocation(BWAPI::Position(start_location_tilepos));
						// TODO function that return enemy base location when found
						_enemy_base_location = true;
					}
				}
			}

			if (!BWAPI::Broodwar->isExplored(start_location_tilepos))
			{

				if (!scout || !start_location_tilepos.isValid())
				{
					return;
				}

				//BWAPI::Broodwar->sendText("command requested");

				// todo work on a fix, may be related to !isExplored -> 
				// can't get back in loop when the tiles have been explored

				/*if (scout->isUnderAttack())
				{
				BWAPI::Broodwar->sendText("Found enemy base location");
				InformationManager::setEnemyStartingBaseLocation(BWAPI::Position(start_location->getPosition()));
				}

				if (scout->isStartingAttack())
				{
				BWAPI::Broodwar->sendText("Scout is attacking");
				InformationManager::setEnemyStartingBaseLocation(BWAPI::Position(start_location->getPosition()));
				}*/

				BWAPI::Broodwar->drawCircleMap(scout->getPosition(), 5, BWAPI::Colors::Blue);

				// if we have issued a command to this unit already this frame, ignore this one
				if (scout->getLastCommandFrame() >= BWAPI::Broodwar->getFrameCount() || scout->isAttackFrame())
				{
					return;
				}

				BWAPI::Broodwar->drawLineMap(scout->getPosition(), BWAPI::Position(start_location_tilepos), BWAPI::Colors::White);

				// get the unit's current command
				BWAPI::UnitCommand currentCommand(scout->getLastCommand());

				// if we've already told this unit to move to this position, ignore this command
				if ((currentCommand.getType() == BWAPI::UnitCommandTypes::Move)
					&& (currentCommand.getTargetPosition() == BWAPI::Position(start_location_tilepos))
					&& scout->isMoving())
				{
					return;
				}

				// if nothing prevents it, attack the target
				scout->move(BWAPI::Position(start_location_tilepos));

				return;
			}
		}
	}
	// TODO move within space of region and
	// get angle of scout to order next actions
	

	scout->move(moveToQuadrant());
	//scout->move(BWAPI::Position(scout->getPosition().x, scout->getPosition().y));

}

BWAPI::Position Scout::moveToQuadrant()
{
	const double PI = std::atan(1.0) * 4;
	double angle = scout->getAngle() * 180 / PI;

	int scout_x = scout->getPosition().x;
	int scout_y = scout->getPosition().y;

	//BWAPI::Broodwar->sendText("tile pos: (%d, %d): ", scout->getTilePosition().x, scout->getTilePosition().y);
	
	int pos_x = 0;
	int pos_y = 0;

	if (angle >= 0 && angle < 90)
	{
		pos_x += 32;
	}
	else if (angle >= 90 && angle < 180)
	{
		pos_x -= 32;
	}
	else if (angle >= 180 && angle < 270)
	{
		pos_y += 32;
	}
	else if (angle >= 270 && angle < 360)
	{
		pos_y -= 32;
	}
	// TODO validate new position (is in the map boundaries) and is inside the enemy base
	// reward function to signal when to leave based on feature weights

	BWTA::Region *enemy_region = BWTA::getRegion(InformationManager::Instance().getEnemyBaseLocation());
	
	// TODO
	// get nearest unwalkable position
	// create a wrapper of the bwta api
	/*BWTA::Polygon p = enemy_region->getPolygon();
	for (size_t j {0}; j < p.size(); ++j)
	{
		BWAPI::Position point1 = p[j];
		BWAPI::Position point2 = p[(j + 1) % p.size()];
		
	}*/
	
	// TODO if the distance is bigger in one direction, 
	// put more weight on other directions
	// allowing the scout to unstuck itself
	// int direction = l + r + t + b;
	
	if (!BWAPI::Broodwar->isWalkable(scout_x + pos_x, scout_y + pos_y))
	{
		std::srand(std::time(nullptr));

		int map_width = BWAPI::Broodwar->mapWidth();
		int map_height = BWAPI::Broodwar->mapHeight();

		int t_x = scout->getTilePosition().x;
		int t_y = scout->getTilePosition().y;
		
		int rnd_x = std::rand() % (map_width - t_x);
		int rnd_y = std::rand() % (map_height - t_y);

		bool rnd_px = std::rand() % 1;
		bool rnd_py = std::rand() % 1;

		if (rnd_px == 0 && rnd_py == 0)
		{
			return BWAPI::Position(scout_x - (rnd_x * TILE_SIZE), scout_y - (rnd_y * TILE_SIZE));
		}
		else if (rnd_px == 0 && rnd_py == 1)
		{
			return BWAPI::Position(scout_x - (rnd_x * TILE_SIZE), scout_y + (rnd_y * TILE_SIZE));
		}
		else if (rnd_px == 1 && rnd_py == 0)
		{
			return BWAPI::Position(scout_x + (rnd_x * TILE_SIZE), scout_y - (rnd_y * TILE_SIZE));
		}
		else if (rnd_px == 1 && rnd_py == 1)
		{
			return BWAPI::Position(scout_x + (rnd_x * TILE_SIZE), scout_y + (rnd_y * TILE_SIZE));
		}
	}
	return BWAPI::Position(scout_x + pos_x, scout_y + pos_y);
}

// TODO function that returns the type of building/type of unit
// map into each type of units

bool Scout::isEnemyBuilding()
{
	for (auto unit : BWAPI::Broodwar->enemy()->getUnits())
	{
		if (unit->getPosition())
		{
			// check if it isn't in it
			for (auto & enemy_building : InformationManager::Instance().enemy_buildings)
			{
				if (enemy_building == unit)
				{
					continue;
				}
				else
				{
					Unit building(unit);
					addEnemyBuilding(building);
				}
			}

		}
	}
}

void Scout::addEnemyBuilding(Unit unit)
{
	InformationManager::Instance().enemy_buildings.push_back(unit);
	BWAPI::Broodwar->sendText("Found enemy %s", unit.unit->getType().c_str());
}