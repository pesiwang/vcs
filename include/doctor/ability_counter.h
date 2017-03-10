
#ifndef _DOCTOR_ABILITY_COUNTER_H
#define _DOCTOR_ABILITY_COUNTER_H

#include "doctor_ability.h"
#include <string>
#include <map>

class AbilityCounter : public DoctorAbility{
	public:
		AbilityCounter(){
			this->abilityName = "counter";
			pthread_mutex_init(&(this->_mapMutex), NULL);
		}
		virtual ~AbilityCounter(){
			pthread_mutex_destroy(&(this->_mapMutex));
		}
		std::string process(std::vector<std::string>);
		std::string help();

	public:
		void incr(const std::string& key);
		void clear(const std::string& key);
		void del(const std::string& key);

		void syncIncr(const std::string& key);
		void syncClear(const std::string& key);
		void syncDel(const std::string& key);

	private:
		std::string _syncGet(const std::string& key);
		std::string _syncGetAll();

		pthread_mutex_t _mapMutex;
		std::map<std::string, unsigned long long> _countMap;
};
#endif

