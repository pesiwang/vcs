#include "doctor/ability_config.h"
#include "util/server_config.h"
#include "event_base/event_base.h"
#include <sstream>
#include <stdlib.h>
#include <strings.h>

using namespace std;
using namespace vcs;

string AbilityConfig::process(vector<string> paramVector){/*{{{*/
	if(paramVector.size() <= 0){
		return "error: no config cmd\n";
	}
	string configCmd	= paramVector[0];

	if(paramVector.size() <= 1){
		return "error: no config key\n";
	}
	string configKey	= paramVector[1];

	log_debug("config cmd is %s", configCmd.c_str());
	if(configCmd.compare("get") == 0){
		return this->_get(configKey);
	} else if(configCmd.compare("set") == 0){
		if(paramVector.size() <= 2){
			return "error: no value type\n";
		}
		string valueType = paramVector[2];

		if(paramVector.size() <= 3){
			return "error: no value\n";
		}
		string value = paramVector[3];
		log_debug("config set %s %s %s", configKey.c_str(), valueType.c_str(), value.c_str());
		return this->_set(configKey, valueType, value);
	} else {
		return "error: unknown config cmd\n";
	}
}/*}}}*/

string AbilityConfig::help(){/*{{{*/
	stringstream ss;

	ss << "config usage:\n"
		<< "config get configKey\n"
		<< "config set configKey valueType[int,string,double,bool] value\n\n";

	return ss.str();
}/*}}}*/

string AbilityConfig::_get(const string& key){/*{{{*/
	stringstream ss;
	ServerConfig* sc = Singleton<ServerConfig>::instance();
	try {
		string value = sc->getStrValue(key);
		ss << key << "\t: " << value << "\n";
		return ss.str();
	} catch(...){
		// do nothing	
	}

	try {
		int value = sc->getIntValue(key);
		ss << key << "\t: " << value << "\n";
		return ss.str();
	} catch(...){
		// do nothing
	}

	try {
		double value = sc->getDoubleValue(key);
		ss << key << "\t: " << value << "\n";
		return ss.str();
	} catch(...){
		// do nothing
	}

	try {
		bool value = sc->getBoolValue(key);
		ss << key << "\t: " << value << "\n";
		return ss.str();
	} catch(...){
		// do nothing
	}

	ss << "error: no config for " << key << "\n";

	return ss.str();
}/*}}}*/

string AbilityConfig::_set(const string& key, const string& valueType, const string& value){/*{{{*/
	stringstream ss;
	ServerConfig* sc = Singleton<ServerConfig>::instance();

	if(valueType.compare("int") == 0){
		int v = atoi(value.c_str());
		sc->setIntValue(key, v);	
	} else if(valueType.compare("string") == 0){
		sc->setStrValue(key, value);	
	} else if(valueType.compare("double") == 0){
		double v = atof(value.c_str());
		sc->setDoubleValue(key, v);
	} else if(valueType.compare("bool") == 0){
		bool v;
		if(strcasecmp(value.c_str(), "true") == 0){
			v = true;
		} else if(strcasecmp(value.c_str(), "false") == 0){
			v = false;
		} else {
			return "error: illegal bool value, only support[true|false]\n";
		}
		sc->setBoolValue(key, v);
	} else {
		return "error: unknown value type\n";
	}

	return "success\n";
}/*}}}*/

