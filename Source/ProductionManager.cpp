#include "ProductionManager.h"

using namespace MagBot;

ProductionManager::ProductionManager()
	: _reserved_minerals(0)
	, _reserved_gas(0)
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
	//_queue.queueAsLowestPriority(MetaType(BWAPI::UnitTypes::Protoss_Nexus), false);
	//_queue.queueAsLowestPriority(MetaType(BWAPI::UnitTypes::Protoss_Gateway), true);

	// TODO reserve minerals/gas
	// TODO fix in this case want to build same buildings 2 times, have enough resources and producer ok but
	// removes both from the queue
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
		bool can_make = false;
		//bool canMake = canMakeNow(producer, currentItem.metaType);

		// TODO check reserved minerals and gas
		if (getFreeMinerals() >= current_item.meta_type.mineralPrice()
			&& getFreeGas() >= current_item.meta_type.gasPrice()) // BWAPI::Broodwar->self()->minerals() , BWAPI::Broodwar->self()->gas()
		{
			can_make = true;
		}
		//BWAPI::Broodwar->sendText("Queue not empty");
		//BWAPI::Broodwar->sendText("%d", canMake);

		// TODO when we have enough resources and an available producer, ex: probe for gateway with minerals >= 150
		// it automatically remove from the queue even if we cannot build it yet because pylon hasnt finish, etc.
		// Solution -> check reserve minerals and gas -> not current minerals/gas
		// since it is checked every frame, temp variable for buildings and build when can 
		// and skip current highest item

		if (producer && can_make)
		{
			// TODO if its a upgrade or tech\
			// TODO FIX reserveMinerals/reservedGas
			
			//reservedMinerals += currentItem.metaType.getUnitType().mineralPrice();
			//reservedGas += currentItem.metaType.getUnitType().gasPrice();

			if (create(producer, current_item))
			{
				//reservedMinerals -= currentItem.metaType.getUnitType().mineralPrice();
				//reservedGas -= currentItem.metaType.getUnitType().gasPrice();
				
				_queue.removeCurrentHighestPriorityItem();

				if (current_item.meta_type.isUnit())
				{
					BWAPI::Broodwar->sendText("%s confirmed and removed from queue", current_item.meta_type.getUnitType().c_str());
				}
			}
			
			// TODO if its a upgrade or tech
			
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
			//BWAPI::Broodwar->sendText("Inserted producersAvailable");
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

	// if we don't care where the unit is return the first one we have
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

bool ProductionManager::create(BWAPI::Unit producer, const BuildOrderItem & item)
{
	if (!producer)
	{
		return false;
	}

	MetaType meta_type = item.meta_type;

	// TODO if isBuildings -> moveWorker by calculating gathering rate, distance and time to travel

	if (meta_type.isBuilding())
	{
		//BWAPI::Broodwar->sendText("Worker is building : %s", t.getUnitType().c_str());
		return (WorkerManager::Instance().build(meta_type.getUnitType()));
	}
	else if (meta_type.isUnit())
	{
		producer->train(meta_type.getUnitType());
		return true;
	}
	// TODO if tech or upgrade
	/*
	else if (t.isTech())
	{
		producer->research(t.getTechType());
	}
	else if (t.isUpgrade())
	{
		producer->upgrade(t.getUpgradeType());
	}
	*/

	return false;
}

int ProductionManager::getFreeMinerals() const
{
	return (BWAPI::Broodwar->self()->minerals() - _reserved_minerals);
}

int ProductionManager::getFreeGas() const
{
	return (BWAPI::Broodwar->self()->gas() - _reserved_gas);
}

// TODO pass queue in parameter?
void ProductionManager::showProductionQueue()
{
	for (size_t i(0); i < _queue.size(); ++i)
	{
		BWAPI::Broodwar->drawTextScreen(200, (i * 10) + 130, "%s", _queue[i].meta_type.getUnitType().c_str());
	}

	BWAPI::Broodwar->drawTextScreen(200, 120, "Number of skipped item: %d", _queue.num_skipped_items);

	BWAPI::Broodwar->drawTextScreen(360, 40, "Reserved Minerals: %d", _reserved_minerals);
	BWAPI::Broodwar->drawTextScreen(360, 50, "Reserved Gas: %d", _reserved_gas);
	BWAPI::Broodwar->drawTextScreen(360, 60, "Free Minerals: %d", getFreeMinerals());
	BWAPI::Broodwar->drawTextScreen(360, 70, "Free Gas: %d", getFreeGas());
}


ProductionManager & ProductionManager::Instance()
{
	static ProductionManager instance;
	return instance;
}