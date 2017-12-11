#include "MagBotModule.h"
#include <iostream>

using namespace MagBot;

// todo onStart -> initiate all valid units to their respective structure

// onUnitCreate -> append unit to their respective structure

void MagBotModule::onStart()
{
	BWAPI::Broodwar << "The map is " << BWAPI::Broodwar->mapName() << "!" << std::endl;
 
	BWAPI::Broodwar->sendText("%s", Config::Paths::Data.c_str());

	BWAPI::Broodwar->setLocalSpeed(Config::BWAPIOptions::SetLocalSpeed);
	BWAPI::Broodwar->setFrameSkip(Config::BWAPIOptions::SetFrameSkip);

	if (Config::BWAPIOptions::EnableCompleteMapInformation)
	{
		BWAPI::Broodwar->enableFlag(BWAPI::Flag::CompleteMapInformation);
	}

	// Enable the UserInput flag, which allows us to control the bot and type messages.
	if (Config::BWAPIOptions::EnableUserInput)
	{
		BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);
	}

	if (Config::BotInfo::PrintInfoOnStart)
	{
		BWAPI::Broodwar->printf("Hello! I am %s, written by %s", Config::BotInfo::BotName.c_str(), Config::BotInfo::Author.c_str());
	}

	// TODO initialize information manager

	// Set the command optimization level so that common commands can be grouped
	// and reduce the bot's APM (Actions Per Minute).
	BWAPI::Broodwar->setCommandOptimizationLevel(2);

	if (Config::Modules::UsingGameCommander)
	{
		BWTA::readMap();
		BWTA::analyze();
	}
}

void MagBotModule::onEnd(bool is_winner)
{
	// Called when the game ends
	if (is_winner)
	{
		// Log your win here!
	}
	InformationManager::Instance().update();
}

void MagBotModule::onFrame()
{
	if (Config::Modules::UsingGameCommander)
	{
		drawTerrainData();
	}

	/*
	// Return if the game is a replay or is paused
	if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
		return;

	// Prevent spamming by only running our onFrame once every number of latency frames.
	// Latency frames are the number of frames before commands are processed.
	if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
		return;
	*/
	
	if (Config::DebugInfo::DrawAllInfo)
	{
		BWAPI::Broodwar->drawTextScreen(0, 30, "Frame Count : %d", BWAPI::Broodwar->getFrameCount());
		BWAPI::Broodwar->drawTextScreen(0, 40, "APM : %d", BWAPI::Broodwar->getAPM());
	}

	WorkerManager::Instance().update();

	ProductionManager::Instance().update();

	BuildingManager::Instance().update();

	SquadManager::Instance().update();

	_game_commander.update();

	if (Config::DebugInfo::DrawAllInfo)
	{
		BWAPI::Broodwar->drawTextScreen(360, 0, "FPS: %d", BWAPI::Broodwar->getFPS());
		BWAPI::Broodwar->drawTextScreen(360, 20, "Average FPS: %f", BWAPI::Broodwar->getAverageFPS());
	}
}

void MagBotModule::onSendText(std::string text)
{
	// Send the text to the game if it is not being processed.
	BWAPI::Broodwar->sendText("%s", text.c_str());
}

void MagBotModule::onReceiveText(BWAPI::Player player, std::string text)
{
	BWAPI::Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void MagBotModule::onPlayerLeft(BWAPI::Player player)
{
	// Interact verbally with the other players in the game by
	// announcing that the other player has left.
	BWAPI::Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void MagBotModule::onNukeDetect(BWAPI::Position target)
{
	if (target)
	{
		BWAPI::Broodwar << "Nuclear Launch Detected at " << target << std::endl;
	}
	else
	{
		BWAPI::Broodwar->sendText("Where's the nuke?");
	}
}

void MagBotModule::onUnitDiscover(BWAPI::Unit unit)
{
}

void MagBotModule::onUnitEvade(BWAPI::Unit unit)
{
}

void MagBotModule::onUnitShow(BWAPI::Unit unit)
{
	// TODO add onUnitShow in infomanager (buildings)
}

void MagBotModule::onUnitHide(BWAPI::Unit unit)
{
}

void MagBotModule::onUnitCreate(BWAPI::Unit unit)
{
	/*if (Broodwar->isReplay())
	{
		// if we are in a replay, then we will print out the build order of the structures
		if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral())
		{
			int seconds = Broodwar->getFrameCount() / 24;
			int minutes = seconds / 60;
			seconds %= 60;
			Broodwar->sendText("%.2d:%.2d: %s creates a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
		}
	}*/
}

void MagBotModule::onUnitDestroy(BWAPI::Unit unit)
{
	// TODO add onUnitDestroy in infomanager (enemy buildings)
	WorkerManager::Instance().onUnitDestroy(unit);
	BuildingManager::Instance().onUnitDestroy(unit);
	SquadManager::Instance().onUnitDestroy(unit);
}

void MagBotModule::onUnitMorph(BWAPI::Unit unit)
{
	/*if (Broodwar->isReplay())
	{
		// if we are in a replay, then we will print out the build order of the structures
		if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral())
		{
			int seconds = Broodwar->getFrameCount() / 24;
			int minutes = seconds / 60;
			seconds %= 60;
			Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
		}
	}*/
}

void MagBotModule::onUnitRenegade(BWAPI::Unit unit)
{
}

void MagBotModule::onSaveGame(std::string game_name)
{
	BWAPI::Broodwar << "The game was saved to \"" << game_name << "\"" << std::endl;
}

void MagBotModule::onUnitComplete(BWAPI::Unit unit)
{
	if (unit->getType().isWorker() && unit->getPlayer() == BWAPI::Broodwar->self())
	{
		BWAPI::Broodwar->sendText("Probe created");
		// TODO make multithreading optimization on ressource collection
		// then call WorkerManager method
		// if not calling on update()
	}
}

void MagBotModule::drawTerrainData()
{
	// iterate through all the base locations, and draw their outlines.
	for (const auto & base_location : BWTA::getBaseLocations())
	{
		BWAPI::TilePosition p = base_location->getTilePosition();

		// draw outline of center location
		BWAPI::Position left_top(p.x * TILE_SIZE, p.y * TILE_SIZE);
		BWAPI::Position right_bottom(left_top.x + 4 * TILE_SIZE, left_top.y + 3 * TILE_SIZE);
		BWAPI::Broodwar->drawBoxMap(left_top, right_bottom, BWAPI::Colors::Blue);

		// draw a circle at each mineral patch
		for (const auto & mineral : base_location->getStaticMinerals())
		{
			BWAPI::Broodwar->drawCircleMap(mineral->getInitialPosition(), 30, BWAPI::Colors::Cyan);
		}

		// draw the outlines of vespene geysers
		for (const auto & geyser : base_location->getGeysers())
		{
			BWAPI::TilePosition p1 = geyser->getInitialTilePosition();
			BWAPI::Position left_top1(p1.x * TILE_SIZE, p1.y * TILE_SIZE);
			BWAPI::Position right_bottom1(left_top1.x + 4 * TILE_SIZE, left_top1.y + 2 * TILE_SIZE);
			BWAPI::Broodwar->drawBoxMap(left_top1, right_bottom1, BWAPI::Colors::Orange);
		}

		// if this is an island expansion, draw a yellow circle around the base location
		if (base_location->isIsland())
		{
			BWAPI::Broodwar->drawCircleMap(base_location->getPosition(), 80, BWAPI::Colors::Yellow);
		}
	}

	// iterate through all the regions
	for (const auto & region : BWTA::getRegions())
	{
		// draw the polygon outline of it in green
		BWTA::Polygon p = region->getPolygon();
		for (size_t j {0}; j < p.size(); ++j)
		{
			BWAPI::Position point1 = p[j];
			BWAPI::Position point2 = p[(j + 1) % p.size()];
			BWAPI::Broodwar->drawLineMap(point1, point2, BWAPI::Colors::Green);
		}
		// visualize the chokepoints with red lines
		for (auto const & chokepoint : region->getChokepoints())
		{
			BWAPI::Position point1 = chokepoint->getSides().first;
			BWAPI::Position point2 = chokepoint->getSides().second;
			BWAPI::Broodwar->drawLineMap(point1, point2, BWAPI::Colors::Red);

			int chokepoint_radius = (int)(chokepoint->getWidth() / 2);
			BWAPI::Broodwar->drawCircleMap(chokepoint->getCenter(), chokepoint_radius, BWAPI::Colors::Teal, false);

		}
	}
}
