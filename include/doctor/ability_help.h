
#ifndef _DOCTOR_ABILITY_HELP_H
#define _DOCTOR_ABILITY_HELP_H

#include "doctor_ability.h"
#include <string>

class AbilityHelp : public DoctorAbility {
	public:
		AbilityHelp(){this->abilityName = "help";}
		virtual ~AbilityHelp(){}
		std::string process(std::vector<std::string>);
		std::string help();

	private:
		std::string _help();

	
};

#endif

