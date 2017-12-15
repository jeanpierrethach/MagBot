#include "MineralNode.h"

using namespace MagBot;

MineralNode::MineralNode()
{
}

void MineralNode::initializePatch()
{
	for (auto & min : _mineralNodes)
	{
		WorkerNodeDeque wnd;
		wnd.patch = min;
		wnd.patch_id = min->getID();
		_deque_workers.emplace_back(wnd);
		if (Config::OptimizationOptions::ParallelAssignmentMining)
		{
			mutexes.emplace_back();
		}
	}
}

void MineralNode::insertWorkerToPatch(WorkerMining worker, int patch_id)
{
	for (auto & node : _deque_workers)
	{
		if (node.patch_id == patch_id)
		{
			node.deque.emplace_back(worker);
		}
	}
}

void MineralNode::insertWorkerToPatchParallel(WorkerMining worker, int patch_id)
{
	int count = 0;
	for (auto & node : _deque_workers)
	{
		if (node.patch_id == patch_id)
		{
			std::lock_guard<std::mutex> lock(mutexes[count]);
			node.deque.emplace_back(worker);
		}
		count++;
	}
}

void MineralNode::displayWorkerinDeque()
{
	int count = 0;
	for (const auto & node : _deque_workers)
	{
		BWAPI::Broodwar->drawTextScreen(360, 180 + (count * 10),
			"patch_id: %d, size: %d", node.patch_id, node.deque.size());
		count++;
		//BWAPI::Broodwar->sendText("patch_id: %d, size: %d", node.patch_id, node.deque.size());
	}
}

WorkerMining & MineralNode::getWorkerMining(BWAPI::Unit unit)
{
	for (auto & node : _deque_workers)
	{
		for (auto & worker : node.deque)
		{
			if (worker.getWorkerID() == unit->getID())
			{
				return worker;
			}
		}
	}
}

const WorkerMining & MineralNode::getWorkerMining(BWAPI::Unit unit) const
{
	for (const auto & node : _deque_workers)
	{
		for (const auto & worker : node.deque)
		{
			if (worker.getWorkerID() == unit->getID())
			{
				return worker;
			}
		}
	}
}

const WorkerState MineralNode::getWorkerState(BWAPI::Unit unit) const
{
	for (const auto & node : _deque_workers)
	{
		for (const auto & worker : node.deque)
		{
			if (worker.getWorkerID() == unit->getID())
			{
				WorkerState state = worker.state;
				return state;
			}
		}
	}
	return WorkerState::NONE;
}

void MineralNode::setWorkerState(BWAPI::Unit unit, WorkerState state)
{
	getWorkerMining(unit).state = state;
}

const BWAPI::Unit MineralNode::getAssignedMineralPatch(BWAPI::Unit unit) const
{
	return getWorkerMining(unit).mineral_patch;
}

void MineralNode::setFrameStartMining(BWAPI::Unit unit, int frame)
{
	getWorkerMining(unit).frame_start_mining = frame;
}

const int MineralNode::getFrameStartMining(BWAPI::Unit unit)
{
	return getWorkerMining(unit).frame_start_mining;
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