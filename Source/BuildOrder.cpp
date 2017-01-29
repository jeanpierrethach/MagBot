#include "BuildOrder.h"

using namespace MagBot;

BuildOrder::BuildOrder()
	: _race(BWAPI::Races::None)
{
}


BuildOrder::~BuildOrder()
{
}



BuildOrder::BuildOrder(const BWAPI::Race & race)
	: _race(race)
{
}

BuildOrder::BuildOrder(const BWAPI::Race & race, const std::vector<MetaType> & meta_vector)
	: _race(race)
	, _build_order(meta_vector)
{
}

void BuildOrder::add(const MetaType & meta_type)
{
	_build_order.push_back(meta_type);
}

void BuildOrder::add(const MetaType & meta_type, int amount)
{
	for (int i = 0; i < amount; ++i)
	{
		_build_order.push_back(meta_type);
	}
}

void BuildOrder::add(const MetaType & meta_type, int amount, int at_supply)
{
	// TODO move this condition to the ProductionManager
	if (BWAPI::Broodwar->self()->supplyUsed() >= (at_supply * 2))
	{
		for (int i = 0; i < amount; ++i)
		{
			_build_order.push_back(meta_type);
		}
	}	
}

const BWAPI::Race & BuildOrder::getRace() const
{
	return _race;
}

const size_t BuildOrder::size() const
{
	return _build_order.size();
}

const MetaType & BuildOrder::operator [] (const size_t & index) const
{
	return _build_order[index];
}

MetaType & BuildOrder::operator [] (const size_t & index)
{
	return _build_order[index];
}