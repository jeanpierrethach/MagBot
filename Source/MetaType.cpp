#include "MetaType.h"

using namespace MagBot;

MetaType::MetaType()
{
}

/*MetaType::MetaType(const std::string & name)
	: _type(MetaTypes::Default)
	, _race(BWAPI::Races::None)
{
}*/

MetaType::MetaType(BWAPI::UnitType unit_type)
	: _unit_type(unit_type)
	, _type(MetaTypes::UNIT)
	, _race(unit_type.getRace())
{
}

MetaType::MetaType(BWAPI::TechType tech_type)
	: _tech_type(tech_type)
	, _type(MetaTypes::TECH)
	, _race(tech_type.getRace())
{
}

MetaType::MetaType(BWAPI::UpgradeType upgrade_type)
	: _upgrade_type(upgrade_type)
	, _type(MetaTypes::UPGRADE)
	, _race(upgrade_type.getRace())
{
}

MetaType::~MetaType()
{
}

const size_t & MetaType::type() const
{
	return _type;
}

bool MetaType::isUnit() const
{
	return _type == MetaTypes::UNIT;
}

bool MetaType::isTech() const
{
	return _type == MetaTypes::TECH;
}

bool MetaType::isUpgrade() const
{
	return _type == MetaTypes::UPGRADE;
}

const BWAPI::Race & MetaType::getRace() const
{
	return _race;
}

bool MetaType::isBuilding()	const
{
	return _type == MetaTypes::UNIT && _unit_type.isBuilding();
}

bool MetaType::isRefinery()	const
{
	return isBuilding() && _unit_type.isRefinery();
}

const BWAPI::UnitType & MetaType::getUnitType() const
{
	return _unit_type;
}

const BWAPI::TechType & MetaType::getTechType() const
{
	return _tech_type;
}

const BWAPI::UpgradeType & MetaType::getUpgradeType() const
{
	return _upgrade_type;
}

int MetaType::supplyRequired()
{
	if (isUnit())
	{
		return _unit_type.supplyRequired();
	}
	else
	{
		return 0;
	}
}

int MetaType::mineralPrice() const
{
	return isUnit() ? _unit_type.mineralPrice() : (isTech() ? _tech_type.mineralPrice() : _upgrade_type.mineralPrice());
}

int MetaType::gasPrice() const
{
	return isUnit() ? _unit_type.gasPrice() : (isTech() ? _tech_type.gasPrice() : _upgrade_type.gasPrice());
}

BWAPI::UnitType MetaType::whatBuilds() const
{
	return isUnit() ? _unit_type.whatBuilds().first : (isTech() ? _tech_type.whatResearches() : _upgrade_type.whatUpgrades());
}

std::string MetaType::getName() const
{
	if (isUnit())
	{
		return _unit_type.getName();
	}
	else if (isTech())
	{
		return _tech_type.getName();
	}
	else if (isUpgrade())
	{
		return _upgrade_type.getName();
	}
	else
	{
		return "MetaType not found";
	}
}