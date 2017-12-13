#pragma once

#include "Common.h"
#include "BWTA.h"
#include <fstream>
#include <iostream>
#include <string>

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

		std::ofstream _mineral_data_file;

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
		BWAPI::Position getStartingBaseLocation();

		void openFileData(std::ofstream & file, std::string path);
		void writeData(std::ofstream & file);
		void writeData();
		void closeFileData(std::ofstream & file);
		void onClose();

		void update();

		void onStart();	

		static InformationManager & Instance();
	};

}

