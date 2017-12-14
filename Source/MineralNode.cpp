#include "MineralNode.h"

using namespace MagBot;

MineralNode::MineralNode()
{
}

void MineralNode::initializePatch()
{
	for (auto & min : _mineralNodes)
	{
		WorkerNodeDeque dwq;
		dwq.patch = min;
		dwq.patch_id = min->getID();
		_deque_workers.emplace_back(dwq);
		if (Config::TestOptions::ParallelAssignmentMining)
		{
			mutexes.emplace_back();
		}
	}
}

void MineralNode::insertWorkerToPatch(WorkerMining worker, int patch_id)
{
	for (auto & d : _deque_workers)
	{
		if (d.patch_id == patch_id)
		{
			d.deque.emplace_back(worker);
		}
	}
}

void MineralNode::insertWorkerToPatchParallel(WorkerMining worker, int patch_id)
{
	int count = 0;
	for (auto & d : _deque_workers)
	{
		if (d.patch_id == patch_id)
		{
			std::lock_guard<std::mutex> lock(mutexes[count]);
			d.deque.emplace_back(worker);
		}
		count++;
	}
}

void MineralNode::displayWorkerinDeque()
{
	int count = 0;
	for (const auto & d : _deque_workers)
	{
		BWAPI::Broodwar->drawTextScreen(360, 180 + (count * 10),
			"patch_id: %d, size: %d", d.patch_id, d.deque.size());
		count++;
		//BWAPI::Broodwar->sendText("patch_id: %d, size: %d", d.patch_id, d.deque.size());
	}
}

WorkerMining & MineralNode::getWorkerMining(BWAPI::Unit unit)
{
	for (auto & d : _deque_workers)
	{
		for (auto & w : d.deque)
		{
			if (w.getWorkerID() == unit->getID())
			{
				return w;
			}
		}
	}
}

const WorkerMining & MineralNode::getWorkerMining(BWAPI::Unit unit) const
{
	for (const auto & d : _deque_workers)
	{
		for (const auto & w : d.deque)
		{
			if (w.getWorkerID() == unit->getID())
			{
				return w;
			}
		}
	}
}

const WorkerState MineralNode::getWorkerState(BWAPI::Unit unit) const
{
	for (const auto & d : _deque_workers)
	{
		for (const auto & w : d.deque)
		{
			if (w.getWorkerID() == unit->getID())
			{
				WorkerState state = w.state;
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
	for (const auto & node : _deque_workers)
	{
		for (const auto & worker : node.deque)
		{
			if (worker.getWorkerID() == unit->getID())
			{
				return worker.mineral_patch;
			}
		}
	}
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