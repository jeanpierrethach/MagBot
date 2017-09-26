#pragma once

#include "Common.h"

#include <stdint.h>

namespace MagBot
{
	struct MineralPatch
	{
		int id;
		int current_min;
		uint8_t current_workers;
	};

	class MineralNode
	{
		std::deque<MineralPatch> _mineral_patch;

	public:
		MineralNode();
		~MineralNode();

		std::deque<MineralPatch> getMineralPatch();
		int getCurrentMineral(int id);
		uint8_t getCurrentWorkers(int id);
	};

}
