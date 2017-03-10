#ifndef _DOCTOR_ABILITY_CONFIG_H
#define _DOCTOR_ABILITY_CONFIG_H

#include "doctor_ability.h"
#include <string>

class AbilityConfig : public DoctorAbility {
	public:
		AbilityConfig(){this->abilityName = "config";}
		virtual ~AbilityConfig(){}
		std::string process(std::vector<std::string>);
		std::string help();

	private:
		std::string _get(const std::string& key);
		std::string _set(const std::string& key, const std::string& valueType, const std::string& value);
};


#endif

