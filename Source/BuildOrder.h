#pragma once

#include "Common.h"

#include "MetaType.h"

namespace MagBot
{

	class BuildOrder
	{
		BWAPI::Race _race;
		std::vector<MetaType> _build_order;

	public:
		BuildOrder();
		~BuildOrder();
		BuildOrder(const BWAPI::Race & race);
		BuildOrder(const BWAPI::Race & race, const std::vector<MetaType> & meta_vector);

		void add(const MetaType & meta_type);
		void add(const MetaType & meta_type, int amount);
		void add(const MetaType & meta_type, int amount, int at_supply);

		const size_t size() const;
		const BWAPI::Race & getRace() const;

		const MetaType & operator [] (const size_t & index) const;
		MetaType & operator [] (const size_t & index);

		// buildStructure or build
		// buildUnit
		// research
		// upgrade
		// TODO if structure doesn't exist/dependencies -> do the necessary to build them then build what you need
	};

}