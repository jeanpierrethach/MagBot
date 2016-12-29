#pragma once

#include "Common.h"
#include "BuildOrderQueue.h"
#include "BuildOrder.h"
#include "WorkerManager.h"
#include <BWTA.h>

namespace MagBot
{
	typedef std::pair<MetaType, size_t> MetaPair;
	typedef std::vector<MetaPair> MetaPairVector;

	struct Strategy
	{
		std::string _name;
		BuildOrder _build_order;

		Strategy(const std::string & name, const BuildOrder & build_order)
			: _name(name)
			, _build_order(build_order)
		{
		}
	};

	class StrategyManager
	{
		std::map<std::string, Strategy> _strategies;
		// const MetaPairVector getProtossBuildOrderGoal();

	public:
		StrategyManager();
		~StrategyManager();

		// void addStrategy(const std::string & name, Strategy & strategy);
		// static StrategyManager & Instance();
		// const MetaPairVector getBuildOrderGoal();
		
	};

}