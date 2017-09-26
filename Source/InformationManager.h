#pragma once

#include "Common.h"
#include "BWTA.h"

namespace MagBot
{

	class InformationManager
	{
		bool _has_spent_resources;
		bool _has_reserved_resources;
		double _minerals_rate;
		double _gas_rate;

		BWAPI::Position _enemy_base_location;
		BWAPI::Position _starting_base_location;

	public:
		InformationManager();
		~InformationManager();

		// gathering rate from start of match to current frame
		double calculateMineralRate();
		double calculateGasRate();

		// todo gathering rate per sec

		double calculateMineralRatePerSec();
		double calculateGasRatePerSec();

		void setEnemyStartingBaseLocation(const BWAPI::Position pos);
		void setStartingBaseLocation(const BWAPI::Position base_location);


	};

}

