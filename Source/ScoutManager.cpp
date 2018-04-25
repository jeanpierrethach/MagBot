#include "ScoutManager.h"

using namespace MagBot;


ScoutManager::ScoutManager()
{
}


ScoutManager::~ScoutManager()
{
}

// TODO Release scout after his task is done? or keep moving around, add gas steal?
// TODO bug to fix when getBuilder from workermanager, it can take this scout unit
// TODO add info about enemy race, etc... (scouting information in information manager)

void ScoutManager::update()
{
	if (!_worker_scout.scout || !_worker_scout.scout->exists() || !(_worker_scout.scout->getHitPoints() > 0))
	{
		return;
	}
	_worker_scout.scoutBaseLocations();
	_worker_scout.updateLocation();
}



ScoutManager & ScoutManager::Instance()
{
	static ScoutManager instance;
	return instance;
}

void ScoutManager::setWorkerScout(BWAPI::Unit unit)
{
	// if we have a previous worker scout, release it back to the worker manager
	/*if (_worker_scout.scout)
	{
		WorkerManager::Instance().setWorkerFree(_worker_scout.scout);
	}*/

	// TODO fix null value of scout when searching
	// for available unit

	_worker_scout.scout = unit;
	WorkerManager::Instance().setWorkerScout(_worker_scout.scout);
}
