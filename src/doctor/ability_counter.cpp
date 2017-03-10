#include "event_base/event_base.h"
#include "doctor/ability_counter.h"
#include <sstream>

using namespace std;

string AbilityCounter::process(vector<string> paramVector){/*{{{*/
	if(paramVector.size() <= 0){
		return "error: no counter cmd\n";
	}

	string counterCmd	= paramVector[0];
	log_debug("counter cmd is %s", counterCmd.c_str());
	if(counterCmd.compare("get") == 0){
		if(paramVector.size() == 1){
			return this->_syncGetAll();
		} else {
			string counterKey = paramVector[1];
			return this->_syncGet(counterKey);
		}
	} else {
		return "error: unknown counter cmd\n";
	}
}/*}}}*/

string AbilityCounter::help(){/*{{{*/
	stringstream ss;

	ss << "counter usage:\n"
		<< "counter get\n"
		<< "counter get counterKey\n";

	return ss.str();
}/*}}}*/

void AbilityCounter::syncIncr(const string& key){/*{{{*/
	pthread_mutex_lock(&(this->_mapMutex));
	this->incr(key);
	pthread_mutex_unlock(&(this->_mapMutex));
}/*}}}*/

void AbilityCounter::syncClear(const string& key){/*{{{*/
	pthread_mutex_lock(&(this->_mapMutex));
	this->clear(key);
	pthread_mutex_unlock(&(this->_mapMutex));
}/*}}}*/

void AbilityCounter::syncDel(const string& key){/*{{{*/
	pthread_mutex_lock(&(this->_mapMutex));
	this->del(key);
	pthread_mutex_unlock(&(this->_mapMutex));
}/*}}}*/

void AbilityCounter::incr(const string& key){/*{{{*/
	map<string, unsigned long long>::iterator iter = this->_countMap.find(key);
	if(iter != this->_countMap.end()){
		iter->second++;	
	} else {
		this->_countMap.insert(pair<string, unsigned long long>(key, 1));
	}
}/*}}}*/

void AbilityCounter::clear(const string& key){/*{{{*/
	map<string, unsigned long long>::iterator iter = this->_countMap.find(key);
	if(iter != this->_countMap.end()){
		iter->second = 0;	
	}
}/*}}}*/

void AbilityCounter::del(const string& key){/*{{{*/
	map<string, unsigned long long>::iterator iter = this->_countMap.find(key);
	if(iter != this->_countMap.end()){
		this->_countMap.erase(iter);
	}
}/*}}}*/

string AbilityCounter::_syncGet(const string& key){/*{{{*/
	pthread_mutex_lock(&(this->_mapMutex));
	map<string, unsigned long long>::iterator iter = this->_countMap.find(key);
	stringstream ss;
	if(iter != this->_countMap.end()){
		ss << iter->first << " : " << iter->second << "\n";
	} else {
		ss << "error: counter key " << key << " not exist\n";
	}
	pthread_mutex_unlock(&(this->_mapMutex));

	return ss.str();
}/*}}}*/

string AbilityCounter::_syncGetAll(){/*{{{*/
	pthread_mutex_lock(&(this->_mapMutex));
	stringstream ss;
	if(this->_countMap.size() <= 0){
		ss << "error: no counter key found\n";
		pthread_mutex_unlock(&(this->_mapMutex));
		return ss.str();
	}

	map<string, unsigned long long>::iterator iter;
	for(iter = this->_countMap.begin(); iter != this->_countMap.end(); ++iter){
		ss << iter->first << "\t: " << iter->second << "\n";
	}

	pthread_mutex_unlock(&(this->_mapMutex));
	return ss.str();
}/*}}}*/

