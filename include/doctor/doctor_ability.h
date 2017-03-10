#ifndef _DOCTOR_ABILITY_H
#define _DOCTOR_ABILITY_H

#include <string>
#include <vector>

class DoctorAbility {
	public:
		std::string abilityName;

	public:
		virtual std::string process(std::vector<std::string>) = 0;
		virtual std::string help() = 0;
};



#endif

