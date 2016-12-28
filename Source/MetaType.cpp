#include "MetaType.h"

using namespace MagBot;

MetaType::MetaType()
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
	return _type == MetaTypes::Unit;
}

bool MetaType::isTech() const
{
	return _type == MetaTypes::Tech;
}

bool MetaType::isUpgrade() const
{
	return _type == MetaTypes::Upgrade;
}

const BWAPI::Race & MetaType::getRace() const
{
	return _race;
}

bool MetaType::isBuilding()	const
{
	return _type == MetaTypes::Unit && _unitType.isBuilding();
}

bool MetaType::isRefinery()	const
{
	return isBuilding() && _unitType.isRefinery();
}

const BWAPI::UnitType & MetaType::getUnitType() const
{
	return _unitType;
}

const BWAPI::TechType & MetaType::getTechType() const
{
	return _techType;
}

const BWAPI::UpgradeType & MetaType::getUpgradeType() const
{
	return _upgradeType;
}

int MetaType::supplyRequired()
{
	if (isUnit())
	{
		return _unitType.supplyRequired();
	}
	else
	{
		return 0;
	}
}

int MetaType::mineralPrice() const
{
	return isUnit() ? _unitType.mineralPrice() : (isTech() ? _techType.mineralPrice() : _upgradeType.mineralPrice());
}

int MetaType::gasPrice() const
{
	return isUnit() ? _unitType.gasPrice() : (isTech() ? _techType.gasPrice() : _upgradeType.gasPrice());
}

BWAPI::UnitType MetaType::whatBuilds() const
{
	return isUnit() ? _unitType.whatBuilds().first : (isTech() ? _techType.whatResearches() : _upgradeType.whatUpgrades());
}

std::string MetaType::getName() const
{
	if (isUnit())
	{
		return _unitType.getName();
	}
	else if (isTech())
	{
		return _techType.getName();
	}
	else if (isUpgrade())
	{
		return _upgradeType.getName();
	}
	else
	{
		return "MetaType not found";
	}
}