#include "MagBotModule.h"
#include <iostream>

using namespace MagBot;
using namespace BWAPI;
using namespace Filter;

void MagBotModule::onStart()
{
	Broodwar << "The map is " << Broodwar->mapName() << "!" << std::endl;

	// Set our BWAPI options here    
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

	// Set the command optimization level so that common commands can be grouped
	// and reduce the bot's APM (Actions Per Minute).
	// TODO may remove this
	Broodwar->setCommandOptimizationLevel(2);

	if (Config::Modules::UsingGameCommander)
	{
		BWTA::readMap();
		BWTA::analyze();
	}

}

void MagBotModule::onEnd(bool isWinner)
{
	// Called when the game ends
	if (isWinner)
	{
		// Log your win here!
	}
}

void MagBotModule::onFrame()
{
	// TO UNCOMMENT when finished with debugging
	/*
	// Return if the game is a replay or is paused
	if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
		return;

	// Prevent spamming by only running our onFrame once every number of latency frames.
	// Latency frames are the number of frames before commands are processed.
	if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
		return;
		*/

	// Called once every game frame
	if (Config::DebugInfo::DrawAllInfo)
	{
		Broodwar->drawTextScreen(0, 30, "Frame Count : %d", Broodwar->getFrameCount());
	}
		
	// initialize a worker manager
	WorkerManager::Instance().update();

	static int lastCheckedGateway = 0;
	
	// TODO check frames, if too fast it may try to build more than what wanted, have to use buildOrderQueue
	if (BWAPI::Broodwar->self()->minerals() >= 150 && lastCheckedGateway + 42 < Broodwar->getFrameCount())
	{
		lastCheckedGateway = Broodwar->getFrameCount();
		WorkerManager::Instance().build(UnitTypes::Protoss_Gateway);
	}

	BuildingManager::Instance().update();

	// TODO: builder may be stuck for few frames...

	// TODO: do same thing for building pylons, etc...

	// TODO build pylons depending of current production rate

	// TODO limit nbOfGateways built 


	// Display the game frame rate as text in the upper left area of the screen
	if (Config::DebugInfo::DrawAllInfo)
	{
		Broodwar->drawTextScreen(360, 0, "FPS: %d", Broodwar->getFPS());
		Broodwar->drawTextScreen(360, 20, "Average FPS: %f", Broodwar->getAverageFPS());
	}

	// Iterate through all the units that we own
	for (auto &u : Broodwar->self()->getUnits())
	{
		if (u->getType().isResourceDepot()) // A resource depot is a Command Center, Nexus, or Hatchery
		{

			// Order the depot to construct more workers! But only when it is idle.
			if (u->isIdle() && !u->train(u->getType().getRace().getWorker()))
			{
				// If that fails, draw the error at the location so that you can visibly see what went wrong!
				// However, drawing the error once will only appear for a single frame
				// so create an event that keeps it on the screen for some frames
				Position pos = u->getPosition();
				Error lastErr = Broodwar->getLastError();
				Broodwar->registerEvent([pos, lastErr](Game*){ Broodwar->drawTextMap(pos, "%c%s", Text::White, lastErr.c_str()); },   // action
					nullptr,    // condition
					Broodwar->getLatencyFrames());  // frames to run

				// Retrieve the supply provider type in the case that we have run out of supplies
				UnitType supplyProviderType = u->getType().getRace().getSupplyProvider();
				static int lastChecked = 0;

				// If we are supply blocked and haven't tried constructing more recently
				if (lastErr == Errors::Insufficient_Supply &&
					lastChecked + 42 < Broodwar->getFrameCount() &&
					Broodwar->self()->incompleteUnitCount(supplyProviderType) == 0)
				{
					lastChecked = Broodwar->getFrameCount();

					// Retrieve a unit that is capable of constructing the supply needed
					Unit supplyBuilder = u->getClosestUnit(GetType == supplyProviderType.whatBuilds().first &&
						(IsIdle || IsGatheringMinerals) &&
						IsOwned);
					// If a unit was found
					if (supplyBuilder)
					{
						if (supplyProviderType.isBuilding())
						{
							TilePosition targetBuildLocation = Broodwar->getBuildLocation(supplyProviderType, supplyBuilder->getTilePosition());
							if (targetBuildLocation)
							{
								// Register an event that draws the target build location
								if (Config::DebugInfo::DrawAllInfo)
								{
									Broodwar->registerEvent([targetBuildLocation, supplyProviderType](Game*)
									{
										Broodwar->drawBoxMap(Position(targetBuildLocation),
											Position(targetBuildLocation + supplyProviderType.tileSize()),
											Colors::Blue);
									},
										nullptr,  // condition
										supplyProviderType.buildTime() + 100);  // frames to run
								}

								// Order the builder to construct the supply structure
								supplyBuilder->build(supplyProviderType, targetBuildLocation);
							}
						}
						else
						{
							// Train the supply provider (Overlord) if the provider is not a structure
							supplyBuilder->train(supplyProviderType);
						}
					} // closure: supplyBuilder is valid
				} // closure: insufficient supply
			} // closure: failed to train idle unit

		}

	} // closure: unit iterator
}

void MagBotModule::onSendText(std::string text)
{

	// Send the text to the game if it is not being processed.
	Broodwar->sendText("%s", text.c_str());


	// Make sure to use %s and pass the text as a parameter,
	// otherwise you may run into problems when you use the %(percent) character!

}

void MagBotModule::onReceiveText(BWAPI::Player player, std::string text)
{
	// Parse the received text
	Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void MagBotModule::onPlayerLeft(BWAPI::Player player)
{
	// Interact verbally with the other players in the game by
	// announcing that the other player has left.
	Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void MagBotModule::onNukeDetect(BWAPI::Position target)
{

	// Check if the target is a valid position
	if (target)
	{
		// if so, print the location of the nuclear strike target
		Broodwar << "Nuclear Launch Detected at " << target << std::endl;
	}
	else
	{
		// Otherwise, ask other players where the nuke is!
		Broodwar->sendText("Where's the nuke?");
	}

	// You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}

void MagBotModule::onUnitDiscover(BWAPI::Unit unit)
{
}

void MagBotModule::onUnitEvade(BWAPI::Unit unit)
{
}

void MagBotModule::onUnitShow(BWAPI::Unit unit)
{
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

void MagBotModule::onSaveGame(std::string gameName)
{
	Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void MagBotModule::onUnitComplete(BWAPI::Unit unit)
{
}
