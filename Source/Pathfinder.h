#pragma once

namespace MagBot
{
	class Pathfinder
	{
	public:

		// TODO add A* algorithm or A* epsilon algorithm for initial pos to target pos,
		// target pos has to be valid terrain (could do max tries)

		// TODO add chokepoints regroup for squads?

		Pathfinder();
		~Pathfinder();
	};

}