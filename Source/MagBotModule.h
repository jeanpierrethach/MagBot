#pragma once

#include <BWAPI.h>
#include <BWTA.h>
#include "GameCommander.h"
#include "BuildingManager.h"
#include "ProductionManager.h"

#include "Config.h"

// Remember not to use "Broodwar" in any global class constructor!

namespace MagBot
{
	class MagBotModule : public BWAPI::AIModule
	{
		GameCommander _game_commander;

	public:
		// Virtual functions for callbacks, leave these as they are.
		virtual void onStart();
		virtual void onEnd(bool is_winner);
		virtual void onFrame();
		virtual void onSendText(std::string text);
		virtual void onReceiveText(BWAPI::Player player, std::string text);
		virtual void onPlayerLeft(BWAPI::Player player);
		virtual void onNukeDetect(BWAPI::Position target);
		virtual void onUnitDiscover(BWAPI::Unit unit);
		virtual void onUnitEvade(BWAPI::Unit unit);
		virtual void onUnitShow(BWAPI::Unit unit);
		virtual void onUnitHide(BWAPI::Unit unit);
		virtual void onUnitCreate(BWAPI::Unit unit);
		virtual void onUnitDestroy(BWAPI::Unit unit);
		virtual void onUnitMorph(BWAPI::Unit unit);
		virtual void onUnitRenegade(BWAPI::Unit unit);
		virtual void onSaveGame(std::string game_name);
		virtual void onUnitComplete(BWAPI::Unit unit);
		// Everything below this line is safe to modify.

		void drawTerrainData();
	};

}