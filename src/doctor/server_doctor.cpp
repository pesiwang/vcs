#include "doctor/server_doctor.h"
#include "doctor/doctor_event.h"
#include "doctor/ability_counter.h"
#include "doctor/ability_status.h"
#include "doctor/ability_config.h"
#include "doctor/ability_help.h"
#include "event_base/event_base.h"
#include <stdlib.h>
#include <sstream>

using namespace vcs;
using namespace std;

ServerDoctor* ServerDoctor::instance(){/*{{{*/
	static ServerDoctor self;
	return &self;
}/*}}}*/

void ServerDoctor::init(const char* host, unsigned short port){/*{{{*/
	this->addAbility(Singleton<AbilityCounter>::instance());
	this->addAbility(Singleton<AbilityStatus>::instance());
	this->addAbility(Singleton<AbilityConfig>::instance());
	this->addAbility(Singleton<AbilityHelp>::instance());

	DoctorListener* listener = Singleton<DoctorListener>::instance();
	listener->fd    = Helper::Socket::listen(host, port);
	if(listener->fd < 0) {
		log_error("listen on %s:%u fail", host, port);
		exit(EXIT_FAILURE);
	}

	EventBase::instance()->add(listener);
	log_info("doctor listen on %s:%u", host, port);
}/*}}}*/

string ServerDoctor::process(const string& abilityName, vector<string> paramVector){/*{{{*/
	vector<DoctorAbility*>::iterator iter;
	for(iter = this->abilityVector.begin(); iter != this->abilityVector.end(); ++iter){
		if(abilityName.compare((*iter)->abilityName) == 0){
			return (*iter)->process(paramVector);
		}
	}

	stringstream ss;
	ss << "error: no " << abilityName << " ability";
	return ss.str(); 
}/*}}}*/

void ServerDoctor::addAbility(DoctorAbility* ability){/*{{{*/
	this->abilityVector.push_back(ability);
}/*}}}*/

DoctorAbility* ServerDoctor::ability(const string& abilityName){/*{{{*/
	vector<DoctorAbility*>::iterator iter;
	for(iter = this->abilityVector.begin(); iter != this->abilityVector.end(); ++iter){
		if(abilityName.compare((*iter)->abilityName) == 0){
			return (*iter);
		}
	}

	return NULL;
}/*}}}*/

