#include "MineralNode.h"

using namespace MagBot;

MineralNode::MineralNode()
{
}


MineralNode::~MineralNode()
{
}


std::map<int, std::deque<WorkerMining> > MineralNode::getMineralPatch()
{
	return _mineral_patch;
}
/*
int MineralNode::getCurrentMineral(int id)
{
	return _mineral_patch[id].current_min;
}

uint8_t MineralNode::getCurrentWorkers(int id)
{
	return _mineral_patch[id].current_workers;
}
*/

void MineralNode::initializeMineralPatches()
{
	//TODO use mineral count at starting base location
	for (auto & mineral_node : _mineralNodes)
	{
		std::deque<WorkerMining> deque;

		int id = mineral_node->getID();
		_mineral_patch[id] = deque;
	}
}

void MineralNode::insertMineralPatch(BWAPI::Unit unit)
{
	_mineralNodes.insert(unit);
}

void MineralNode::removeMineralPatch(BWAPI::Unit unit)
{
	_mineralNodes.erase(unit);
}

BWAPI::Unitset MineralNode::getMineralNodes()
{
	return _mineralNodes;
}

/*bool MineralNode::hasWorker(WorkerMining worker, int patch_id)
{
	const std::deque<WorkerMining>::const_iterator it;

	for (auto it = _mineral_patch[patch_id].cbegin(); it < _mineral_patch[patch_id].cend();)
	{
		if (it->worker_id == worker.getWorker()->getID())
		{
			return true;
		}
	}
	return false;
}*/

void MineralNode::insertWorkerToPatch(WorkerMining worker, int patch_id)//, std::map<int, std::deque<WorkerMining>> & mineral_nodes)
{
	// if worker isn't already in the deque

	//if (!hasWorker)
	//{
	//mineral_nodes[patch_id].push_back(worker);
	_mineral_patch[patch_id].push_back(worker);
	BWAPI::Broodwar->sendText("patch id: %d, size: %d", patch_id, _mineral_patch[patch_id].size());
	//}
}

// TODO : add test
void MineralNode::removeWorkerFromPatch(BWAPI::Unit unit)
{
	if (!unit) return;

	int min_id = getMineralIDFrom(unit);
	if (min_id != -1)
	{
		_mineral_patch[min_id].pop_front();
	}
}

int MineralNode::getMineralIDFrom(BWAPI::Unit unit)
{
	for (auto patch : _mineral_patch)
	{
		for (auto worker : patch.second)
		{
			if (unit->getID() == worker.getWorker()->getID())
			{
				return worker.getMineralPatchID();
			}
		}
	}
	return -1;
}