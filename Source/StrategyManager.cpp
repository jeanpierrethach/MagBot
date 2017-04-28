#include "StrategyManager.h"

using namespace MagBot;

StrategyManager::StrategyManager()
{
}


StrategyManager::~StrategyManager()
{
}

StrategyManager & StrategyManager::Instance()
{
	static StrategyManager instance;
	return instance;
}

void StrategyManager::addStrategy(const std::string & name, Strategy & strategy)
{
	_strategies[name] = strategy;
}

BuildOrder StrategyManager::getOpeningBuildOrder()
{
	// http://wiki.teamliquid.net/starcraft/2_Gate_Reaver_(vs._Protoss)#cite_note-range-8
	BuildOrder build_order;

	MetaType zealot{BWAPI::UnitTypes::Protoss_Zealot};
	MetaType dragoon{BWAPI::UnitTypes::Protoss_Dragoon};
	MetaType pylon{BWAPI::UnitTypes::Protoss_Pylon};
	MetaType gateway{BWAPI::UnitTypes::Protoss_Gateway};
	MetaType probe{BWAPI::UnitTypes::Protoss_Probe};
	MetaType cyber{BWAPI::UnitTypes::Protoss_Cybernetics_Core};
	MetaType dragoon_range{BWAPI::UpgradeTypes::Singularity_Charge};
	MetaType robo{BWAPI::UnitTypes::Protoss_Robotics_Facility};
	MetaType assimilator{BWAPI::UnitTypes::Protoss_Assimilator};

	// TODO add continuously if probe count < value? then after a certain frame count check if nb idle workers >= number then expand
	// TODO when pushing in queue, check if blocking or not since we may have enough resources to build structures, but no supply
	// for units
	//build_order.add(probe, 4);
	build_order.add(pylon, 1);
	build_order.add(gateway, 1);
	build_order.add(assimilator, 1);
	build_order.add(zealot, 1);
	build_order.add(pylon, 1);
	build_order.add(cyber, 1);
	build_order.add(zealot, 1);
	build_order.add(pylon, 1);
	build_order.add(dragoon, 1);
	build_order.add(gateway, 1);
	build_order.add(dragoon, 1);
	build_order.add(pylon, 1);
	build_order.add(dragoon_range, 1);
	build_order.add(robo, 1);

	// TODO only activate the item when supply is at the correct supply used
	/*build_order.add(probe, 4);
	build_order.add(pylon, 1, 8);
	build_order.add(gateway, 1, 10);
	build_order.add(assimilator, 1, 12);
	build_order.add(zealot, 1, 13);
	build_order.add(pylon, 1, 16);
	build_order.add(cyber, 1, 18);
	build_order.add(zealot, 1, 19);
	build_order.add(pylon, 1, 22);
	build_order.add(dragoon, 1, 23);
	build_order.add(gateway, 1, 26);
	build_order.add(dragoon, 1, 27);
	build_order.add(pylon, 1, 28);
	build_order.add(dragoon_range, 1, 29);
	build_order.add(robo, 1, 36);*/

	return build_order;
}