#include "ProductionManager.h"

using namespace MagBot;

ProductionManager::ProductionManager()
{
	setBuildOrder(StrategyManager::Instance().getOpeningBuildOrder());

	// TODO add(unit_type, nb)
	// TODO MetaType m(" ", nb, " ", nb, " " nb);

	// queueItem(BuildOrderItem b)?

	// highest at the front, lowest at the back

	// TODO more testing, put probe in priority with false and next item is different but blocking == true and blocking == false
	/*_queue.queueAsHighestPriority(MetaType(BWAPI::UnitTypes::Protoss_Probe), true);
	_queue.queueAsLowestPriority(MetaType(BWAPI::UnitTypes::Protoss_Pylon), true);

	// blocking used to skip item if cant build when reaching the item, keep true if cant skip item
	_queue.queueAsLowestPriority(MetaType(BWAPI::UnitTypes::Protoss_Nexus), false);
	_queue.queueAsLowestPriority(MetaType(BWAPI::UnitTypes::Protoss_Gateway), true);
	_queue.queueAsLowestPriority(MetaType(BWAPI::UnitTypes::Protoss_Zealot), true);
	*/
}

ProductionManager::~ProductionManager()
{
}

// TODO add a bool for blocking or not, here and in the strategymanager.cpp / buildorder.h add() function
void ProductionManager::setBuildOrder(const BuildOrder & build_order)
{
	_queue.clearAll();

	for (size_t i(0); i < build_order.size(); ++i)
	{
		_queue.queueAsLowestPriority(build_order[i], true);
	}
}

void ProductionManager::update()
{
	// TODO create typedef? or using? for BWAPI::UnitTypes:: ...

	// TODO move these strategies somewhere..
	for (auto & depot : BWAPI::Broodwar->self()->getUnits())
	{
		if (depot->getType().isResourceDepot())
		{
			BWAPI::UnitType probe = BWAPI::UnitTypes::Protoss_Probe;
			if (!depot->isTraining() && canMakeNow(depot, MetaType(probe)) 
				&& BWAPI::Broodwar->self()->completedUnitCount(probe) < 21) // TODO modify if want to expand
			{
				_queue.queueAsHighestPriority(MetaType(probe), false);
			}
		}
	}

	for (auto & building : BWAPI::Broodwar->self()->getUnits())
	{
		if (building->getType() == BWAPI::UnitTypes::Protoss_Gateway)
		{
			// TODO check dependencies towards goals, ex: need cyber core for dragoons if count == 0 then add to the queue, etc..
			// add more or check when trying to make?
			/*BWAPI::UnitType cyber = BWAPI::UnitTypes::Protoss_Cybernetics_Core;
			if (BWAPI::Broodwar->self()->completedUnitCount(cyber) == 0) //!_queue.hasItem(cyber) && 
			{
				_queue.queueAsHighestPriority(MetaType(cyber), true);
			}*/

			BWAPI::UnitType dragoon = BWAPI::UnitTypes::Protoss_Dragoon;
			if (!building->isTraining() && canMakeNow(building, MetaType(dragoon)))
			{
				static int lastChecked = 0;

				if (_queue.isEmpty())
				{
					_queue.queueAsLowestPriority(MetaType(dragoon), false);
				}
				// TODO modify value/store for strategies + modify 400 into the exact amount to a pylon constructing time?
				// or construct pylon with an offset depending on current used supply.
				else if (lastChecked + 400 < BWAPI::Broodwar->getFrameCount())
				{
					lastChecked = BWAPI::Broodwar->getFrameCount();
					_queue.queueAsLowestPriority(MetaType(dragoon), false);
				}			
			}
			// TODO move constructing pylon outside, be independent of any units, should be checking for supply used + an offset
			// to change during the game depending on production rate/gathering rate/priority on mass production of unit,etc...
			else
			{
				//BWAPI::UnitType supplyProviderType = u->getType().getRace().getSupplyProvider();
				static int lastChecked = 0;
				BWAPI::Error lastErr = BWAPI::Broodwar->getLastError();

				// If we are supply blocked and haven't tried constructing more recently
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

	// TODO add config for other draws/debug in class
	if (Config::DebugInfo::DrawProductionQueue)
	{
		showProductionQueue();
	}
}

void ProductionManager::manageBuildOrderQueue()
{
	if (_queue.isEmpty())
		return;

	// Changed from reference to not affect currentItem in queue when trying to getNextHighestPriorityItem()
	BuildOrderItem current_item = _queue.getHighestPriorityItem();

	while (!_queue.isEmpty())
	{
		BWAPI::Unit producer = getProducer(current_item.meta_type);

		bool can_make = canMakeNow(producer, current_item.meta_type);

		if (producer && can_make)
		{
			if (current_item.meta_type.isUnit())
			{
				BWAPI::Broodwar->sendText("%s confirmed and removed from queue", current_item.meta_type.getUnitType().c_str());
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
	//BWAPI::Broodwar->sendText("Unittype %s", meta_type.getUnitType().c_str());
	//BWAPI::Broodwar->sendText("Producer %s", producer_type.c_str());

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
	int min_dist {0};

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

	// TODO if isBuildings -> moveWorker by calculating gathering rate, distance and time to travel

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
	for (size_t i(0); i < _queue.size(); ++i)
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

ProductionManager & ProductionManager::Instance()
{
	static ProductionManager instance;
	return instance;
}