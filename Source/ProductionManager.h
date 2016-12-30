#pragma once

#include "Common.h"
#include "BuildOrderQueue.h"
#include "BuildingManager.h"
#include "BuildOrder.h"
#include "WorkerManager.h"

namespace MagBot
{

	class ProductionManager
	{
		BuildOrderQueue _queue;

		void setBuildOrder(const BuildOrder & build_order);	
		void manageBuildOrderQueue();
		void create(BWAPI::Unit producer, const BuildOrderItem & item);

		BWAPI::Unit getClosestUnitToPosition(const BWAPI::Unitset & units, BWAPI::Position closest_to);

		bool canMakeNow(BWAPI::Unit producer, MetaType meta_type);
		bool hasEnoughResources(MetaType meta_type);

		int getFreeMinerals() const;
		int getFreeGas() const;

		// void predictWorkerMovement(const Building & b);

	public:
		ProductionManager();
		~ProductionManager();

		void update();
		void showProductionQueue();

		BWAPI::Unit getProducer(MetaType meta_type, BWAPI::Position closest_to = BWAPI::Positions::None);

		static ProductionManager & Instance();
	};

}