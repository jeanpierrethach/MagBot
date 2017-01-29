#pragma once

#include "Common.h"

namespace MagBot
{

	namespace MetaTypes
	{
		enum { UNIT, TECH, UPGRADE, DEFAULT};
	}

	class MetaType
	{
		size_t _type;
		BWAPI::Race _race;
		BWAPI::UnitType _unit_type;
		BWAPI::TechType _tech_type;
		BWAPI::UpgradeType _upgrade_type;

	public:
		MetaType();

		//MetaType(const std::string & name);
		MetaType(BWAPI::UnitType unit_type);
		MetaType(BWAPI::TechType tech_type);
		MetaType(BWAPI::UpgradeType upgrade_type);
		~MetaType();
		
		bool isUnit() const;
		bool isTech() const;
		bool isUpgrade() const;
		//bool isCommand() const;	may be useful for cancelling attacked and low hp constructing buildings
		bool isBuilding() const;
		bool isRefinery() const;

		const size_t & type() const;
		const BWAPI::Race & getRace() const;

		const BWAPI::UnitType & getUnitType() const;
		const BWAPI::TechType & getTechType() const;
		const BWAPI::UpgradeType & getUpgradeType() const;

		int supplyRequired();
		int mineralPrice() const;
		int gasPrice() const;

		BWAPI::UnitType whatBuilds() const;
		std::string getName() const;

	};
}