#include "MineralNode.h"

using namespace MagBot;

MineralNode::MineralNode()
{
}


MineralNode::~MineralNode()
{
}

std::deque<MineralPatch> MineralNode::getMineralPatch()
{
	return _mineral_patch;
}

int MineralNode::getCurrentMineral(int id)
{
	return _mineral_patch[id].current_min;
}

uint8_t MineralNode::getCurrentWorkers(int id)
{
	return _mineral_patch[id].current_workers;
}