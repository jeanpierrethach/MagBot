#pragma once

#include "Common.h"
#include "Squad.h"

namespace MagBot
{

	class SquadManager
	{

		std::map<std::string, Squad> _squads;

	public:
		SquadManager();
		~SquadManager();



	};
}

