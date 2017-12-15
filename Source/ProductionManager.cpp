#include "ProductionManager.h"

using namespace MagBot;

ProductionManager::ProductionManager()
{
	if (Config::StratOptions::GateReaver)
	{
		setBuildOrder(StrategyManager::Instance().getOpeningBuildOrder());
	}
	if (Config::OptimizationOptions::OptimizeMining)
	{
		setBuildOrder(StrategyManager::Instance().optimizeMiningTest());
	}
}


ProductionManager::~ProductionManager()
{
}

void ProductionManager::setBuildOrder(const BuildOrder & build_order)
{
	_queue.clearAll();

	for (size_t i {0}; i < build_order.size(); ++i)
	{
		_queue.queueAsLowestPriority(build_order[i], true);
	}
}

void ProductionManager::setBuildOrderHP(const BuildOrder & build_order)
{
	for (size_t i {0}; i < build_order.size(); ++i)
	{
		_queue.queueAsHighestPriority(build_order[i], false);
	}
}

void ProductionManager::update()
{
	if (Config::StratOptions::TimedExpansion)
	{
		//setBuildOrderHP(StrategyManager::Instance().timedExpansion());
	}

	// TODO require tech building for units
	//BWAPI::UnitType high_templar = BWAPI::UnitTypes::Protoss_High_Templar;
	//addUnitInQueue(MetaType {high_templar});
	//getPrerequiste(MetaType {high_templar});

	for (auto & depot : BWAPI::Broodwar->self()->getUnits())
	{
		if (depot->getType().isResourceDepot())
		{
			BWAPI::UnitType probe = BWAPI::UnitTypes::Protoss_Probe;
			if (!depot->isTraining() && canMakeNow(depot, MetaType{probe})
				&& BWAPI::Broodwar->self()->completedUnitCount(probe) < Config::StratOptions::NbOfWorkers)
			{
				_queue.queueAsHighestPriority(MetaType{probe}, false);
			}
		}
	}

	for (auto & building : BWAPI::Broodwar->self()->getUnits())
	{
		if (building->getType() == BWAPI::UnitTypes::Protoss_Gateway)
		{
			BWAPI::UnitType dragoon = BWAPI::UnitTypes::Protoss_Dragoon;
			if (!building->isTraining() && canMakeNow(building, MetaType{dragoon}))
			{
				static int lastChecked {0};

				if (_queue.isEmpty())
				{
					_queue.queueAsLowestPriority(MetaType{dragoon}, false);
				}
				// TODO modify value/store for strategies + modify 400 into the exact amount to a pylon constructing time?
				// or construct pylon with an offset depending on current used supply.
				else if (lastChecked + 400 < BWAPI::Broodwar->getFrameCount())
				{
					lastChecked = BWAPI::Broodwar->getFrameCount();
					_queue.queueAsLowestPriority(MetaType{dragoon}, false);
				}			
			}
			// TODO move constructing pylon outside, be independent of any units, should be checking for supply used + an offset
			// to change during the game depending on production rate/gathering rate/priority on mass production of unit,etc...
			else
			{
				static int lastChecked {0};
				BWAPI::Error lastErr = BWAPI::Broodwar->getLastError();

				if (lastErr == BWAPI::Errors::Insufficient_Supply &&
					lastChecked + 42 < BWAPI::Broodwar->getFrameCount() &&
					BWAPI::Broodwar->self()->incompleteUnitCount(BWAPI::UnitTypes::Protoss_Pylon) == 0)
				{
					lastChecked = BWAPI::Broodwar->getFrameCount();
					_queue.queueAsHighestPriority(BWAPI::UnitTypes::Protoss_Pylon, true);
				}	
			}
		}
	}

	manageBuildOrderQueue();

	if (Config::DebugInfo::DrawProductionQueue)
	{
		showProductionQueue();
	}
}

void ProductionManager::manageBuildOrderQueue()
{
	if (_queue.isEmpty())
		return;

	BuildOrderItem current_item = _queue.getHighestPriorityItem();

	while (!_queue.isEmpty())
	{
		BWAPI::Unit producer = getProducer(current_item.meta_type);

		bool can_make = canMakeNow(producer, current_item.meta_type);

		if (producer && can_make)
		{
			if (current_item.meta_type.isUnit())
			{
				//BWAPI::Broodwar->sendText("%s confirmed and removed from queue", current_item.meta_type.getUnitType().c_str());
			}
			else if (current_item.meta_type.isTech())
			{
				BWAPI::Broodwar->sendText("%s confirmed and removed from queue", current_item.meta_type.getTechType().c_str());
			}
			else if (current_item.meta_type.isUpgrade())
			{
				BWAPI::Broodwar->sendText("%s confirmed and removed from queue", current_item.meta_type.getUpgradeType().c_str());
			}
			create(producer, current_item);
			_queue.removeCurrentHighestPriorityItem();		
			break;	
		}
		else if (_queue.canSkipItem())
		{
			_queue.skipItem();
			current_item = _queue.getNextHighestPriorityItem();
		}
		else
		{
			break;
		}
	}
}

BWAPI::Unit ProductionManager::getProducer(MetaType meta_type, BWAPI::Position closest_to)
{
	BWAPI::UnitType producer_type = meta_type.whatBuilds();
	BWAPI::Unitset producers_available;

	for (const auto & unit : BWAPI::Broodwar->self()->getUnits())
	{
		if (unit->getType() != producer_type) { continue; }
		if (!unit->isCompleted()) { continue; }
		if (unit->isTraining()) { continue; }
		if (!unit->isPowered()) { continue; }
		if (unit->getType() == producer_type)
		{
			producers_available.insert(unit);
		}
	}
	return getClosestUnitToPosition(producers_available, closest_to);
}

BWAPI::Unit ProductionManager::getClosestUnitToPosition(const BWAPI::Unitset & units, BWAPI::Position closest_to)
{
	if (units.size() == 0)
		return nullptr;

	// return if position doesn't matter
	if (closest_to == BWAPI::Positions::None)
	{
		return *(units.begin());
	}

	BWAPI::Unit closest_unit = nullptr;
	int min_dist {9999};

	for (const auto & unit : units)
	{
		int distance = unit->getDistance(closest_to);
		if (!closest_unit || distance < min_dist)
		{
			closest_unit = unit;
			min_dist = distance;
		}
	}
	return closest_unit;
}

void ProductionManager::create(BWAPI::Unit producer, const BuildOrderItem & item)
{
	if (!producer)
		return;

	MetaType meta_type = item.meta_type;

	if (meta_type.isUnit() && meta_type.getUnitType().isBuilding())
	{
		BuildingManager::Instance().addBuildingTask(meta_type.getUnitType());
	}
	else if (meta_type.isUnit())
	{
		producer->train(meta_type.getUnitType());
	}
	else if (meta_type.isTech())
	{
		producer->research(meta_type.getTechType());
	}
	else if (meta_type.isUpgrade())
	{
		producer->upgrade(meta_type.getUpgradeType());
	}
}

int ProductionManager::getFreeMinerals() const
{
	return (BWAPI::Broodwar->self()->minerals() - BuildingManager::Instance().getReservedMinerals());
}

int ProductionManager::getFreeGas() const
{
	return (BWAPI::Broodwar->self()->gas() - BuildingManager::Instance().getReservedGas());
}

bool ProductionManager::hasEnoughResources(MetaType meta_type)
{
	return (getFreeMinerals() >= meta_type.mineralPrice()) && (getFreeGas() >= meta_type.gasPrice());
}

bool ProductionManager::canMakeNow(BWAPI::Unit producer, MetaType meta_type)
{
	bool can_make = hasEnoughResources(meta_type);
	if (can_make)
	{
		if (meta_type.isUnit())
		{
			can_make = BWAPI::Broodwar->canMake(meta_type.getUnitType(), producer);
		}
		else if (meta_type.isTech())
		{
			can_make = BWAPI::Broodwar->canResearch(meta_type.getTechType(), producer);
		}
		else if (meta_type.isUpgrade())
		{
			can_make = BWAPI::Broodwar->canUpgrade(meta_type.getUpgradeType(), producer);
		}
		else
		{
			return false;
		}
	}
	return can_make;
}

// TODO pass queue in parameter?
void ProductionManager::showProductionQueue()
{
	for (size_t i {0}; i < _queue.size(); ++i)
	{
		if (_queue[i].meta_type.isUnit())
		{
			BWAPI::Broodwar->drawTextScreen(480, (i * 10) + 130, "%s", _queue[i].meta_type.getUnitType().c_str());
		}
		else if (_queue[i].meta_type.isTech())
		{
			BWAPI::Broodwar->drawTextScreen(480, (i * 10) + 130, "%s", _queue[i].meta_type.getTechType().c_str());
		}
		else if (_queue[i].meta_type.isUpgrade())
		{
			BWAPI::Broodwar->drawTextScreen(480, (i * 10) + 130, "%s", _queue[i].meta_type.getUpgradeType().c_str());
		}
	}
	BWAPI::Broodwar->drawTextScreen(480, 110, "Queue size: %d", _queue.size());
	BWAPI::Broodwar->drawTextScreen(480, 120, "Number of skipped item: %d", _queue.getSkippedItemsCount());
	BWAPI::Broodwar->drawTextScreen(360, 40, "Reserved Minerals: %d", BuildingManager::Instance().getReservedMinerals());
	BWAPI::Broodwar->drawTextScreen(360, 50, "Reserved Gas: %d", BuildingManager::Instance().getReservedGas());
	BWAPI::Broodwar->drawTextScreen(360, 60, "Free Minerals: %d", getFreeMinerals());
	BWAPI::Broodwar->drawTextScreen(360, 70, "Free Gas: %d", getFreeGas());
	BWAPI::Broodwar->drawTextScreen(360, 80, "Supply used: %d", BWAPI::Broodwar->self()->supplyUsed());
	BWAPI::Broodwar->drawTextScreen(360, 90, "Supply total: %d", BWAPI::Broodwar->self()->supplyTotal());
}

// require testing -> need to fix insertiong of every prerequiste of the metatype
std::vector<BWAPI::UnitType> ProductionManager::getPrerequiste(const MetaType & meta_type)
{
	std::vector<BWAPI::UnitType> unit_types;
	
	MetaType m_type = meta_type;
	
	//bool has_unit_type = false;
	while (!hasPrerequiste(m_type))
	{
		BWAPI::UnitType producer_type = m_type.whatBuilds();
		//BWAPI::UnitType tech_type = m_type.getUnitType();


		unit_types.push_back(producer_type);
		BWAPI::Broodwar->sendText("%s", producer_type.getName());
		MetaType m_type2 = MetaType(producer_type);
		BWAPI::UnitType new_type = m_type2.whatBuilds();
		m_type = MetaType(new_type);
	}

	return unit_types;
}

bool ProductionManager::hasPrerequiste(const MetaType & meta_type)
{
	BWAPI::UnitType producer_type = meta_type.whatBuilds();
	for (const auto & unit : BWAPI::Broodwar->self()->getUnits())
	{
		if (unit->getType() == producer_type)
		{
			return true;
		}
	}
	return false;
}

// require testing -> todo fix insertion in queue
void ProductionManager::addUnitInQueue(const MetaType & meta_type)
{
	if (hasPrerequiste(meta_type))
	{
		_queue.queueAsLowestPriority(meta_type, false);
	}
	else
	{
		std::vector<BWAPI::UnitType> list_of_units = getPrerequiste(meta_type);

		for (auto it = list_of_units.begin(); it != list_of_units.end();)
		{
			MetaType m_type = MetaType {*it};
			_queue.queueAsLowestPriority(m_type, false);
			list_of_units.erase(it);
		}
	}
}

ProductionManager & ProductionManager::Instance()
{
	static ProductionManager instance;
	return instance;
}