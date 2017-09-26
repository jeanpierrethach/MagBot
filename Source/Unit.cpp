#include "Unit.h"

using namespace MagBot;

void Unit::setPosition(BWAPI::Position pos, bool b)
{
	_pos = pos;
	_is_position_set = b;
}

BWAPI::Position Unit::getPosition()
{
	return _pos;
}

bool Unit::isPositionSet()
{
	return _is_position_set;
}

void Unit::resetPosition()
{
	_is_position_set = false;
}