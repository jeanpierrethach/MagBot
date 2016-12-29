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

		//bool                hasResources(BWAPI::UnitType type);
		//bool                canMake(BWAPI::UnitType type);
		//bool                meetsReservedResources(MetaType type);
		//bool                canMakeNow(BWAPI::Unit producer, MetaType t);

		void setBuildOrder(const BuildOrder & build_order);

		bool create(BWAPI::Unit producer, const BuildOrderItem & item);

		void manageBuildOrderQueue();

		BWAPI::Unit getClosestUnitToPosition(const BWAPI::Unitset & units, BWAPI::Position closest_to);

		int _reserved_minerals;
		int _reserved_gas;

		int getFreeMinerals() const;
		int getFreeGas() const;
		// void                predictWorkerMovement(const Building & b);

	public:
		ProductionManager();
		~ProductionManager();

		void update();

		void showProductionQueue();

		BWAPI::Unit getProducer(MetaType meta_type, BWAPI::Position closest_to = BWAPI::Positions::None);
		static ProductionManager & Instance();
	};

}