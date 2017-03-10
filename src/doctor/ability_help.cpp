#include "doctor/ability_help.h"
#include "doctor/server_doctor.h"
#include <string>
#include <sstream>

using namespace std;

string AbilityHelp::process(vector<string> paramVector){/*{{{*/
	if(paramVector.size() <= 0){
		return this->help();
	}

	string abilityName = paramVector[0];
	DoctorAbility* da = ServerDoctor::instance()->ability(abilityName);
	if(da == NULL){
		return "the doctor do not has this ability, use help to find out the ability";
	}

	return da->help();
}/*}}}*/

string AbilityHelp::help(){/*{{{*/
	stringstream ss;
	ss << "welcome, I am the doctor who has abilities below:\n";

	vector<DoctorAbility*> av = ServerDoctor::instance()->abilityVector;
	vector<DoctorAbility*>::iterator iter;
	int i = 1;
	for(iter = av.begin(); iter != av.end(); ++iter){
		ss << i << ": " << (*iter)->abilityName << "\n";
		i++;
	}

	ss << "\n";
	ss << "use help ability to show detail\n";

	return ss.str();
}/*}}}*/

