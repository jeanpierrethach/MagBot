#include "ScoutManager.h"

using namespace MagBot;


ScoutManager::ScoutManager()
{
}


ScoutManager::~ScoutManager()
{
}

void ScoutManager::update()
{

}


ScoutManager & ScoutManager::Instance()
{
	static ScoutManager instance;
	return instance;
}

void ScoutManager::setWorkerScout(BWAPI::Unit unit)
{
	// if we have a previous worker scout, release it back to the worker manager
	/*if (_workerScout)
	{
		WorkerManager::Instance().finishedWithWorker(_workerScout);
	}*/

	_workerScout = unit;
	//WorkerManager::Instance().setScoutWorker(_workerScout);
}