#pragma once

#include <BWAPI.h>

namespace MagBot
{

	class ConstructorManager
	{
		BWAPI::Unit _constructor_worker;

	public:
		ConstructorManager();
		~ConstructorManager();

		//void build();
		//void isStuck();
		//void isBuilding();
		//void isAttacked();
		//void isDead(); -> remove from worker manager


	};

}