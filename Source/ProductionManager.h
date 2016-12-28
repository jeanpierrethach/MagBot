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

		void setBuildOrder(const BuildOrder & buildOrder);

		void create(BWAPI::Unit producer, BuildOrderItem & item);

		void manageBuildOrderQueue();

		BWAPI::Unit getClosestUnitToPosition(const BWAPI::Unitset & units, BWAPI::Position closestTo);

		//int                 getFreeMinerals();
		//int                 getFreeGas();
		// void                predictWorkerMovement(const Building & b);

	public:
		ProductionManager();
		~ProductionManager();

		void update();

		BWAPI::Unit getProducer(MetaType t, BWAPI::Position closestTo = BWAPI::Positions::None);
		static ProductionManager & Instance();
	};

}