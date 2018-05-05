#include "InformationManager.h"

using namespace MagBot;

InformationManager::InformationManager()
{
}


InformationManager::~InformationManager()
{
}

double InformationManager::calculateMineralRate()
{
	if (BWAPI::Broodwar->getFrameCount() != 0)
	{
		int nb_min = BWAPI::Broodwar->self()->gatheredMinerals();
		int nb_frames = BWAPI::Broodwar->getFrameCount();
		_minerals_rate = (double) nb_min / nb_frames;
		return _minerals_rate;
	}
	return 0;
}

double InformationManager::calculateGasRate()
{
	if (BWAPI::Broodwar->getFrameCount() != 0)
	{
		int nb_gas = BWAPI::Broodwar->self()->gatheredGas();
		int nb_frames = BWAPI::Broodwar->getFrameCount();
		_gas_rate = (double) nb_gas / nb_frames;
		return _gas_rate;
	}
	return 0;
}

void InformationManager::setEnemyStartingBaseLocation(const BWAPI::Position pos)
{
	_enemy_base_location = pos;
}

void InformationManager::setStartingBaseLocation(const BWAPI::Position pos)
{
	_starting_base_location = pos;
}

BWAPI::Position InformationManager::getStartingBaseLocation()
{
	return _starting_base_location;
}

BWAPI::Position InformationManager::getEnemyBaseLocation()
{
	return _enemy_base_location;
}

void InformationManager::openFileData(std::ofstream & file, std::string path)
{
	file.open(path);

	if (file.fail())
	{
		BWAPI::Broodwar->sendText("Couldn't open data write file");
	}
}

void InformationManager::writeData(std::ofstream & file)
{
	std::lock_guard<std::mutex> lock(mutex);
	file << "Frame:" << BWAPI::Broodwar->getFrameCount() << std::setw(3) << "\t"
		 << "Total minerals:"<< BWAPI::Broodwar->self()->gatheredMinerals() << "\n";
}

void InformationManager::closeFileData(std::ofstream & file)
{
	file.close();
}

void InformationManager::onStart()
{
	openFileData(_mineral_data_file, Config::Paths::Data);
}

void InformationManager::update()
{
	writeData(_mineral_data_file);
}

void InformationManager::onClose()
{
	closeFileData(_mineral_data_file);
}

void InformationManager::writeInfo(std::ofstream & file, WorkerMining worker_mining, long long duration)
{
	std::lock_guard<std::mutex> lock(mutex);
	file << "Frame:" << BWAPI::Broodwar->getFrameCount() << std::setw(6) << "\t"
		 << "worker_id:" << worker_mining.worker_id << std::setw(6) << "\t"
		 << "assigned_patch_id:" << worker_mining.mineral_patch_id << std::setw(3) << "\t"
		 << "patch_pos:" << "(" << worker_mining.patch_pos_x << ","
		 << worker_mining.patch_pos_y << ")" << std::setw(5) << "\t"
		 << "RUNTIME of calculateBestPatch:" << duration << " microseconds"
		 << "\n";
}

void InformationManager::log(WorkerMining worker_mining, long long duration)
{
	writeInfo(_mineral_data_file, worker_mining, duration);
}

// TODO
void InformationManager::writeEndGameWinner(bool is_winner)
{
	openFileData(_endgame_data_file, Config::Paths::EndGameData);

	if (is_winner)
	{
		_endgame_data_file << "1" << "\n";
	}
	else
	{
		_endgame_data_file << "-1" << "\n";
	}
	
	closeFileData(_endgame_data_file);
}

InformationManager & InformationManager::Instance()
{
	static InformationManager instance;
	return instance;
}

BWAPI::Unitset InformationManager::getEnemyUnits()
{
	return _enemy_units;
}

void InformationManager::insertEnemyUnit(BWAPI::Unit unit)
{
	_enemy_units.insert(unit);
}