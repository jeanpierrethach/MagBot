#pragma once

#include "Common.h"
#include "BuildOrderQueue.h"
#include "BuildOrder.h"
#include "WorkerManager.h"
#include <BWTA.h>

namespace MagBot
{
	using MetaPair = std::pair<MetaType, size_t>;
	using MetaPairVector = std::vector<MetaPair>;

	struct Strategy
	{
		std::string _name;
		BuildOrder _build_order;
		
		Strategy()
			: _name("None")
		{
		}

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

		void addStrategy(const std::string & name, Strategy & strategy);
		
		// TODO when workers destroyed, calculate enemy visible force, rebuild workers greedly or not
		// up in production? or up in army size?

		// TODO if minerals too big, get workers in all refineries available and transfer to minerals
		// or transfer like 1, 2 per refinery, then if need retransfer
		
		// TODO calculate production rate and build pylons accordingly

		// const MetaPairVector getBuildOrderGoal();

		// const BuildOrder & getOpeningBuildOrder() const;
		BuildOrder getOpeningBuildOrder();
		static StrategyManager & Instance();
		
	};

}