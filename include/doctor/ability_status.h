#ifndef _DOCTOR_ABILITY_STATUS_H
#define _DOCTOR_ABILITY_STATUS_H

#include "doctor_ability.h"
#include <string>

class AbilityStatus : public DoctorAbility {
	public:
		AbilityStatus(){this->abilityName = "status";}
		virtual ~AbilityStatus(){}
		std::string process(std::vector<std::string>);
		std::string help();
		void onTimer();

	private:
		std::string _all();
		std::string _server();
		std::string _worker();

	
};

#endif

