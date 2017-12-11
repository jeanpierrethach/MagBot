#include "MineralNode.h"

using namespace MagBot;

MineralNode::MineralNode()
{
}


MineralNode::~MineralNode()
{
}

void MineralNode::initializePatch()
{
	for (auto & min : _mineralNodes)
	{
		WorkerNodeQueue dw(min, min->getID());
		//dw.patch_id = min->getID();
		_deque_workers.push_back(dw);
	}
}

void MineralNode::insertWorkerToPatch(WorkerMining worker, int patch_id)
{
	for (auto & d : _deque_workers)
	{
		if (d.patch_id == patch_id)
		{
			d.deque.push_back(worker);
		}
	}
}

void MineralNode::displayWorkerinDeque()
{
	int count = 0;
	for (const auto d : _deque_workers)
	{
		BWAPI::Broodwar->drawTextScreen(360, 180 + (count * 10),
			"patch_id: %d, size: %d", d.patch_id, d.deque.size());
		count++;
		//BWAPI::Broodwar->sendText("patch_id: %d, size: %d", d.patch_id, d.deque.size());
	}
}

WorkerState MineralNode::getWorkerState(BWAPI::Unit unit)
{
	for (auto d : _deque_workers)
	{
		for (auto w : d.deque)
		{
			if (w.getWorkerID() == unit->getID())
			{
				WorkerState state = w.state;//w.getState();
				return state;
			}
		}
	}
	return WorkerState::NONE;
}

void MineralNode::setWorkerState(BWAPI::Unit unit, WorkerState state)
{
	for (auto & node : _deque_workers)
	{
		for (auto & worker : node.deque)
		{
			if (worker.getWorkerID() == unit->getID())
			{
				worker.state = state;
				//worker.setState(state);
			}
		}
	}
}

void MineralNode::setFrameStartMining(BWAPI::Unit unit, int frame)
{
	for (auto & node : _deque_workers)
	{
		for (auto & worker : node.deque)
		{
			if (worker.getWorkerID() == unit->getID())
			{
				worker.frame_start_mining = frame;
			}
		}
	}
}

int MineralNode::getFrameStartMining(BWAPI::Unit unit)
{
	for (auto & node : _deque_workers)
	{
		for (auto & worker : node.deque)
		{
			if (worker.getWorkerID() == unit->getID())
			{
				return worker.frame_start_mining;
			}
		}
	}
}

void MineralNode::removeWorkerFromPatch(BWAPI::Unit unit)
{
	for (auto & node : _deque_workers)
	{
		for (auto & worker : node.deque)
		{
			if (worker.getWorkerID() == unit->getID())
			{
				node.deque.pop_front();
			}
		}
	}
}







BWAPI::Unitset MineralNode::getMineralNodes()
{
	return _mineralNodes;
}

void MineralNode::insertMineralPatch(BWAPI::Unit unit)
{
	_mineralNodes.insert(unit);
}




// ----------------------------------

/*std::vector<std::pair<int, std::deque<WorkerMining>> > MineralNode::getMineralPatch()
{
	return _mineral_patch;
}



void MineralNode::initializeMineralPatches()
{
	//TODO use mineral count at starting base location
	for (auto & mineral_node : _mineralNodes)
	{
		std::deque<WorkerMining> deque;
		int id = mineral_node->getID();
		_mineral_patch.push_back(std::make_pair(id, deque));
	}
}



void MineralNode::removeMineralPatch(BWAPI::Unit unit)
{
	_mineralNodes.erase(unit);
}





void MineralNode::insertWorkerToPatch(WorkerMining worker, int patch_id)//, std::map<int, std::deque<WorkerMining>> & mineral_nodes)
{
	// if worker isn't already in the deque

	
	//BWAPI::Broodwar->sendText("%s", typeid(_mineral_patch[patch_id]).name());
	
	_mineral_patch[patch_id].second.push_back(worker);
	BWAPI::Broodwar->sendText("patch id: %d, size: %d", patch_id, _mineral_patch[patch_id].second.size());
	
	//BWAPI::Broodwar->sendText("patch id: %d, size: %d", patch_id, _mineral_patch[patch_id].second.size());
	
	//}
}

// TODO : add test
void MineralNode::removeWorkerFromPatch(BWAPI::Unit unit)
{
	if (!unit) return;

	int min_id = getMineralIDFrom(unit);
	
	if (min_id != -1)
	{
	_mineral_patch[min_id].second.pop_front();
	//	_mineral_patch[min_id].pop_front();
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

*/