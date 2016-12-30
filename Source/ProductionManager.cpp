#include "ProductionManager.h"

using namespace MagBot;

ProductionManager::ProductionManager()
{
	// TODO FIX allow BuildOrder -> related to MetaType class constructor?

	/*BuildOrder b;
	MetaType m1(BWAPI::UnitTypes::Protoss_Zealot);
	b.add(m1);
	b.add(m1);
	setBuildOrder(b);*/

	/*std::vector<MetaType> vecMetaType;
	
	vecMetaType.push_back(MetaType(BWAPI::UnitTypes::Protoss_Zealot));
	vecMetaType.push_back(MetaType(BWAPI::UnitTypes::Protoss_Zealot));
	vecMetaType.push_back(MetaType(BWAPI::UnitTypes::Protoss_Zealot));
	vecMetaType.push_back(MetaType(BWAPI::UnitTypes::Protoss_Zealot));

	BuildOrder b(BWAPI::Races::Protoss, vecMetaType);
	setBuildOrder(b);*/

	// queueItem(BuildOrderItem b)?


	// highest at the front, lowest at the back
	// TODO more testing, put probe in priority with false and next item is different but blocking == true and blocking == false
	_queue.queueAsHighestPriority(MetaType(BWAPI::UnitTypes::Protoss_Probe), true);
	_queue.queueAsHighestPriority(MetaType(BWAPI::UnitTypes::Protoss_Probe), true);
	_queue.queueAsHighestPriority(MetaType(BWAPI::UnitTypes::Protoss_Probe), true);
	_queue.queueAsHighestPriority(MetaType(BWAPI::UnitTypes::Protoss_Probe), true);
	_queue.queueAsLowestPriority(MetaType(BWAPI::UnitTypes::Protoss_Pylon), true);

	// blocking used to skip item if cant build when reaching the item, keep true if cant skip item
	_queue.queueAsLowestPriority(MetaType(BWAPI::UnitTypes::Protoss_Nexus), false);
	_queue.queueAsLowestPriority(MetaType(BWAPI::UnitTypes::Protoss_Gateway), true);
	_queue.queueAsLowestPriority(MetaType(BWAPI::UnitTypes::Protoss_Gateway), true);
	_queue.queueAsLowestPriority(MetaType(BWAPI::UnitTypes::Protoss_Gateway), true);
	_queue.queueAsLowestPriority(MetaType(BWAPI::UnitTypes::Protoss_Zealot), true);
	_queue.queueAsLowestPriority(MetaType(BWAPI::UnitTypes::Protoss_Zealot), true);
	_queue.queueAsLowestPriority(MetaType(BWAPI::UnitTypes::Protoss_Zealot), true);
	_queue.queueAsLowestPriority(MetaType(BWAPI::UnitTypes::Protoss_Zealot), true);
}

ProductionManager::~ProductionManager()
{
}

void ProductionManager::setBuildOrder(const BuildOrder & build_order)
{
	_queue.clearAll();
	BWAPI::Broodwar->sendText("Queue cleared");

	for (size_t i(0); i < build_order.size(); ++i)
	{
		_queue.queueAsLowestPriority(build_order[i], true);
		BWAPI::Broodwar->sendText("Queue added build order");
	}
	BWAPI::Broodwar->sendText("Queue size: %d", build_order.size());
}

void ProductionManager::update()
{
	BWAPI::Broodwar->drawTextScreen(200, 110, "Queue size: %d", _queue.size());

	manageBuildOrderQueue();	

	// TODO add config for this and other draws/debug in class
	showProductionQueue();
}

void ProductionManager::manageBuildOrderQueue()
{
	//BWAPI::Broodwar->sendText("Queue size: %d", _queue.size());
	if (_queue.isEmpty())
	{
		//BWAPI::Broodwar->sendText("Queue empty");
		return;
	}

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
	//BWAPI::Broodwar->sendText("Unittype %s", m.getUnitType().c_str());
	//BWAPI::Broodwar->sendText("Producer %s", producerType.c_str());

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
	{
		return nullptr;
	}

	// return if position doesn't matter
	if (closest_to == BWAPI::Positions::None)
	{
		return *(units.begin());
	}

	BWAPI::Unit closest_unit = nullptr;
	double min_dist(1000000);

	for (const auto & unit : units)
	{
		double distance = unit->getDistance(closest_to);
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
	{
		return;
	}

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
	return (meta_type.mineralPrice() <= getFreeMinerals()) && (meta_type.gasPrice() <= getFreeGas());
}

bool ProductionManager::canMakeNow(BWAPI::Unit producer, MetaType meta_type)
{
	bool canMake = hasEnoughResources(meta_type);
	if (canMake)
	{
		if (meta_type.isUnit())
		{
			canMake = BWAPI::Broodwar->canMake(meta_type.getUnitType(), producer);
		}
		else if (meta_type.isTech())
		{
			canMake = BWAPI::Broodwar->canResearch(meta_type.getTechType(), producer);
		}
		else if (meta_type.isUpgrade())
		{
			canMake = BWAPI::Broodwar->canUpgrade(meta_type.getUpgradeType(), producer);
		}
		else
		{
			return false;
		}
	}
	return canMake;
}

// TODO pass queue in parameter?
void ProductionManager::showProductionQueue()
{
	for (size_t i(0); i < _queue.size(); ++i)
	{
		BWAPI::Broodwar->drawTextScreen(200, (i * 10) + 130, "%s", _queue[i].meta_type.getUnitType().c_str());
	}
	BWAPI::Broodwar->drawTextScreen(200, 120, "Number of skipped item: %d", _queue.num_skipped_items);
	BWAPI::Broodwar->drawTextScreen(360, 40, "Reserved Minerals: %d", BuildingManager::Instance().getReservedMinerals());
	BWAPI::Broodwar->drawTextScreen(360, 50, "Reserved Gas: %d", BuildingManager::Instance().getReservedGas());
	BWAPI::Broodwar->drawTextScreen(360, 60, "Free Minerals: %d", getFreeMinerals());
	BWAPI::Broodwar->drawTextScreen(360, 70, "Free Gas: %d", getFreeGas());
}

ProductionManager & ProductionManager::Instance()
{
	static ProductionManager instance;
	return instance;
}