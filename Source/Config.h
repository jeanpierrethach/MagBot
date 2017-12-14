#pragma once

#include <string>

namespace Config
{
	namespace Paths
	{
		extern std::string Data;
	}
	namespace Modules
	{
		extern bool UsingGameCommander;
		extern bool UsingScoutManager;
	}

	namespace BWAPIOptions
	{
		extern unsigned short int SetLocalSpeed;
		extern unsigned short int SetFrameSkip;
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
		extern bool DrawBuildingsCount;
		extern bool DrawBuildingTilesInfo;
		extern bool DrawBuildingTimeInConstruction;
		extern bool DrawBuildingsOwnedOrDestroyed;
		extern bool DrawWorkerTaskInfo;
		extern bool DrawProductionQueue;
	}

	namespace StratOptions
	{
		extern bool GateReaver;
		extern bool TimedExpansion;
		extern int NbFramesExpansion;
		extern int NbOfWorkers;
	}

	namespace TestOptions
	{
		extern bool OptimizeMining;
		extern bool ParallelAssignmentMining;
	}
}