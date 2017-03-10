#ifndef _SERVER_DOCTOR_H
#define _SERVER_DOCTOR_H

#include <string>
#include <vector>
#include "doctor_ability.h"
#include "ability_counter.h"
#include "ability_config.h"
#include "ability_status.h"

class ServerDoctor{
	public:
		static ServerDoctor* instance();
		void init(const char*, unsigned short);

		std::string process(const std::string& abilityName, std::vector<std::string> paramVector);
		void addAbility(DoctorAbility*);
		DoctorAbility* ability(const std::string& abilityName);

	public:
		std::vector<DoctorAbility*> abilityVector;

	private:
		ServerDoctor(){}
		virtual ~ServerDoctor(){}


};
#endif
