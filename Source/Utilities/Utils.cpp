#include "Utils.h"

using namespace MagBot;

Utils::Utils()
{
}


Utils::~Utils()
{
}

double Utils::radToDeg(double angle)
{
	const double PI = std::atan(1.0) * 4;
	return angle * 180 / PI;
}

double Utils::getAngleDeg(BWAPI::Position p1, BWAPI::Position p2)
{
	const double PI = std::atan(1.0) * 4;
	return atan2(p1.y - p2.y, p1.x - p2.x) * 180 / PI;
}

double Utils::getAngleRad(BWAPI::Position p1, BWAPI::Position p2)
{
	return atan2(p1.y - p2.y, p1.x - p2.x);
}

int Utils::getDistance(BWAPI::Position p1, BWAPI::Position p2)
{
	return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

void Utils::average(BWAPI::Position & pos, int size)
{
	if (size != 0)
	{
		pos.x /= size;
		pos.y /= size;
	}
}

void Utils::normalize(BWAPI::Position & pos)
{
	double length = sqrt((pos.x * pos.x) + (pos.y * pos.y));
	if (length != 0)
	{
		pos.x = (pos.x / length);
		pos.y = (pos.y / length);
	}
}
