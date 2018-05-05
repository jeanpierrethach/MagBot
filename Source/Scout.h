#pragma once

#include <BWAPI.h>
#include <BWTA.h>
#include "Common.h"
#include "InformationManager.h"
#include <cstdlib>


namespace MagBot
{
	enum class State : int { MOVING = 0, CIRCLING = 1, ATTACKING = 2, IDLE = 3 };
	enum class Actions : int { LEFT = 0, RIGHT = 1, UP = 2, DOWN = 3, NONE = 4 };
	enum class Goal : int { LOOP = 0, EXPLORE = 1 };

	class Scout
	{

	private:
		bool _enemy_base_location = false;

	public:
		BWAPI::Unit scout;

		BWAPI::Position enemy_base_location;

		State state;
		Actions actions;
		State goal;

		std::vector<Actions> getActions(State state);

		int heuristicCost(State state, Goal goal);
		int actionCost(Actions action);

		Actions selectAction(State state);

		bool testGoal(State state);

		void scoutBaseLocations();

		BWAPI::Position moveToQuadrant();
		void updateLocation();
		bool isEnemyBuilding();
		void addEnemyBuilding(Unit unit);

		Scout();
		~Scout();
	};


}
