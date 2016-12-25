#pragma once

#include <string>

namespace Config
{
	namespace Modules
	{
		extern bool UsingGameCommander;
		extern bool UsingScoutManager;
	}

	namespace BWAPIOptions
	{
		extern int SetLocalSpeed;
		extern int SetFrameSkip;
		extern bool EnableUserInput;
		extern bool EnableCompleteMapInformation;
	}

	namespace Tournament
	{
		extern int GameEndFrame;
	}

	namespace BotInfo
	{
		extern std::string BotName;
		extern std::string Author;
		extern bool PrintInfoOnStart;
	}

	namespace DebugInfo
	{
		extern bool DrawAllInfo;
		extern bool DrawBuildingTilesInfo;
		extern bool DrawBuildingTimeInConstruction;
		extern bool DrawBuildingsOwnedOrDestroyed;
		extern bool DrawWorkerTaskInfo;
	}

	//namespace config file
	//namespace strategy
	//namespace debug
	//namespace micro
	//namespace macro
	//namespace tools
}