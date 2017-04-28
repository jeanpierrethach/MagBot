#include "Config.h"

namespace Config
{
	namespace Modules
	{
		bool UsingGameCommander = true;
		bool UsingScoutManager = true;
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
	}
}