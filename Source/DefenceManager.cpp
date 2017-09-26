#include "DefenceManager.h"

using namespace MagBot;

DefenceManager::DefenceManager()
{
}


DefenceManager::~DefenceManager()
{
}


void DefenceManager::setCombatUnits()
{
	// detect combat zones
	BWAPI::Position pos;

	for (auto & worker : _workers)
	{
		if (worker->canAttack())
		{
			smartAttackMove(worker, pos);
		}
	}
}

void DefenceManager::assignWorkersTo(BWAPI::Unit unit, WorkerTask task, BWAPI::Position position)
{
	if (!unit)
		return;

	_worker_task[unit] = task;

	smartAttackMove(unit, position);
}

DefenceManager & DefenceManager::Instance()
{
	static DefenceManager instance;
	return instance;
}

