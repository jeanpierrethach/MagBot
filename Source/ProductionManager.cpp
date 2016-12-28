#include "ProductionManager.h"

using namespace MagBot;

ProductionManager::ProductionManager()
{
	// TODO FIX to allow with build order and change logic location from magbotmodule to here
	// for pylons, probes, gateways, etc.

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

	// TODO highestpriority after first created doesn't work proprely

	// TODO queue as highest priority seems to work but now lowest? check if it workds correctly
	
	// TODO change logic of the queue, put highestpriority in front, lowest in the back.
	// always get the front not the back...
	
	
	_queue.queueAsHighestPriority(MetaType(BWAPI::UnitTypes::Protoss_Probe), true);
	//_queue.queueAsHighestPriority(MetaType(BWAPI::UnitTypes::Protoss_Probe), true);
	//_queue.queueAsHighestPriority(MetaType(BWAPI::UnitTypes::Protoss_Probe), true);
	
	_queue.queueAsLowestPriority(MetaType(BWAPI::UnitTypes::Protoss_Zealot), true);
	_queue.queueAsLowestPriority(MetaType(BWAPI::UnitTypes::Protoss_Zealot), true);
	_queue.queueAsLowestPriority(MetaType(BWAPI::UnitTypes::Protoss_Zealot), true);
	_queue.queueAsLowestPriority(MetaType(BWAPI::UnitTypes::Protoss_Zealot), true);
	_queue.queueAsLowestPriority(MetaType(BWAPI::UnitTypes::Protoss_Gateway), true);
	_queue.queueAsLowestPriority(MetaType(BWAPI::UnitTypes::Protoss_Pylon), true);
	
}


ProductionManager::~ProductionManager()
{
}

void ProductionManager::setBuildOrder(const BuildOrder & buildOrder)
{
	_queue.clearAll();
	BWAPI::Broodwar->sendText("Queue cleared");

	for (size_t i(0); i < buildOrder.size(); ++i)
	{
		_queue.queueAsLowestPriority(buildOrder[i], true);
		BWAPI::Broodwar->sendText("Queue added build order");
	}
	BWAPI::Broodwar->sendText("Queue size: %d", buildOrder.size());
}

void ProductionManager::update()
{
	BWAPI::Broodwar->drawTextScreen(200, 110, "Queue size: %d", _queue.size());

	manageBuildOrderQueue();	

	for (int i = 0; i < _queue.size(); ++i)
	{
		BWAPI::Broodwar->drawTextScreen(200, (i * 10) + 130, "%s", _queue[i].metaType.getUnitType().c_str());
	}
}

void ProductionManager::manageBuildOrderQueue()
{
	//BWAPI::Broodwar->sendText("Queue size: %d", _queue.size());
	if (_queue.isEmpty())
	{
		//BWAPI::Broodwar->sendText("Queue empty");
		return;
	}

	BuildOrderItem & currentItem = _queue.getHighestPriorityItem();

	while (!_queue.isEmpty())
	{
		BWAPI::Unit producer = getProducer(currentItem.metaType);
		bool canMake = false;
		//bool canMake = canMakeNow(producer, currentItem.metaType);

		// TODO check reserved minerals and gas
		if (BWAPI::Broodwar->self()->minerals() >= currentItem.metaType.mineralPrice() 
			&& BWAPI::Broodwar->self()->gas() >= currentItem.metaType.gasPrice())
		{
			canMake = true;
		}
		//BWAPI::Broodwar->sendText("Queue not empty");
		//BWAPI::Broodwar->sendText("%d", canMake);

		// TODO when we have enough resources and an available producer, ex: probe for gateway with minerals >= 150
		// it automatically remove from the queue even if we cannot build it yet because pylon hasnt finish, etc.
		// Solution -> reserve minerals and gas, temp variable for buildings and build when can 
		// and skip current highest item
		if (producer && canMake)
		{
			create(producer, currentItem);

			// TODO Move this to create()?
			_queue.removeHighestPriorityItem();
			// TODO if its a upgrade or tech
			if (currentItem.metaType.isUnit())
			{
				BWAPI::Broodwar->sendText("%s confirmed and removed from queue", currentItem.metaType.getUnitType().c_str());
			}	
			break;
		}
		else
		{
			break;
		}
	}
}

BWAPI::Unit ProductionManager::getProducer(MetaType m, BWAPI::Position closestTo)
{
	BWAPI::UnitType producerType = m.whatBuilds();
	//BWAPI::Broodwar->sendText("Unittype %s", m.getUnitType().c_str());
	//BWAPI::Broodwar->sendText("Producer %s", producerType.c_str());

	BWAPI::Unitset producersAvailable;

	for (auto & unit : BWAPI::Broodwar->self()->getUnits())
	{
		if (unit->getType() != producerType) { continue; }
		if (!unit->isCompleted()) { continue; }
		if (unit->isTraining()) { continue; }
		if (!unit->isPowered()) { continue; }
		if (unit->getType() == producerType)
		{
			//BWAPI::Broodwar->sendText("Inserted producersAvailable");
			producersAvailable.insert(unit);
		}
	}

	return getClosestUnitToPosition(producersAvailable, closestTo);
}

BWAPI::Unit ProductionManager::getClosestUnitToPosition(const BWAPI::Unitset & units, BWAPI::Position closestTo)
{
	if (units.size() == 0)
	{
		return nullptr;
	}

	// if we don't care where the unit is return the first one we have
	if (closestTo == BWAPI::Positions::None)
	{
		return *(units.begin());
	}

	BWAPI::Unit closestUnit = nullptr;
	double minDist(1000000);

	for (auto & unit : units)
	{
		double distance = unit->getDistance(closestTo);
		if (!closestUnit || distance < minDist)
		{
			closestUnit = unit;
			minDist = distance;
		}
	}

	return closestUnit;
}

void ProductionManager::create(BWAPI::Unit producer, BuildOrderItem & item)
{
	if (!producer)
	{
		return;
	}

	MetaType t = item.metaType;

	// TODO if isBuildings -> moveWorker by calculating gathering rate, distance and time to travel

	if (t.isBuilding())
	{
		WorkerManager::Instance().build(t.getUnitType());
		BWAPI::Broodwar->sendText("Worker is building : %s", t.getUnitType().c_str());
		
		//_queue.removeHighestPriorityItem();
	}
	else if (t.isUnit())
	{
		producer->train(t.getUnitType());
		//_queue.removeHighestPriorityItem();
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
}

/*
void ProductionManager::drawProductionQueue()
{

}
*/

ProductionManager & ProductionManager::Instance()
{
	static ProductionManager instance;
	return instance;
}