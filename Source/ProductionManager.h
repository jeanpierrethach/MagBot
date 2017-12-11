#pragma once

#include "Common.h"
#include "BuildOrderQueue.h"
#include "BuildingManager.h"
#include "BuildOrder.h"
#include "WorkerManager.h"
#include "StrategyManager.h"

namespace MagBot
{

	class ProductionManager
	{
		BuildOrderQueue _queue;

		void setBuildOrder(const BuildOrder & build_order);	
		void setBuildOrderHP(const BuildOrder & build_order);

		void manageBuildOrderQueue();
		void create(BWAPI::Unit producer, const BuildOrderItem & item);

		BWAPI::Unit getClosestUnitToPosition(const BWAPI::Unitset & units, BWAPI::Position closest_to);

		bool canMakeNow(BWAPI::Unit producer, MetaType meta_type);
		bool hasEnoughResources(MetaType meta_type);
		bool hasPrerequiste(const MetaType & meta_type);
		std::vector<BWAPI::UnitType> getPrerequiste(const MetaType & meta_type);
		void addUnitInQueue(const MetaType & meta_type);

		int getFreeMinerals() const;
		int getFreeGas() const;

	public:
		ProductionManager();
		~ProductionManager();

		void update();
		void showProductionQueue();

		BWAPI::Unit getProducer(MetaType meta_type, BWAPI::Position closest_to = BWAPI::Positions::None);

		static ProductionManager & Instance();
	};

}