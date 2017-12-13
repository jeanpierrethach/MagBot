#include "Config.h"

namespace Config
{
	namespace Paths
	{
		std::string Data = "bwapi-data/write/data.txt";
	}
	namespace Modules
	{
		bool UsingGameCommander = true;
		bool UsingScoutManager = false;
	}

	namespace BWAPIOptions
	{
		unsigned short int SetLocalSpeed = 24;
		unsigned short int SetFrameSkip = 0;
		bool EnableUserInput = true;
		bool EnableCompleteMapInformation = false;
	}

	namespace Tournament
	{
		int GameEndFrame = 86400;
	}

	namespace BotInfo
	{
		std::string BotName = "MagBot";
		std::string Author = "Jean-Pierre Thach";
		bool PrintInfoOnStart = true;
	}

	namespace DebugInfo
	{
		bool DrawAllInfo = true;
		bool DrawBuildingsCount = true;
		bool DrawBuildingTilesInfo = true;
	    bool DrawBuildingTimeInConstruction = true;
		bool DrawBuildingsOwnedOrDestroyed = true;
		bool DrawWorkerTaskInfo = true;
		bool DrawProductionQueue = true;
		bool DrawMineralHandling = true;
	}

	namespace StratOptions
	{
		bool GateReaver = false;
		bool TimedExpansion = true;
		int NbFramesExpansion = 5900;
	    int NbOfWorkers = 18;
	}

	namespace TestOptions
	{
		bool OptimizeMining = true;
	}
}