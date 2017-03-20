
#ifndef _MODEL_GROUP_MODEL_H
#define _MODEL_GROUP_MODEL_H

#include <string>
#include <map>
#include "group_storage.h"

#define GROUP_HASH_KEY "group"
#define GROUP_DELETED_HASH_KEY "group_deleted"

class GroupModel {
	public:
		enum GROUP_STATUS {
			GROUP_STATUS_NORMAL		= 0,
			GROUP_STATUS_GRAY		= 1,
			GROUP_STATUS_BLACK		= 2
		};

		class MemberInfo {
			public:
				MemberInfo() : activeTime(0){}
				time_t activeTime;
				time_t joinTime;
		};

		class GroupInfo {
			public:
				inline void addMember(const std::string& id);
				inline void addMember(const std::string& id, const GroupModel::MemberInfo& memberInfo);
				inline bool memberExist(const std::string& id);
				inline void delMember(const std::string& id);
				inline void updateMemberActiveTime(const std::string& id, time_t);
				inline time_t memberActiveTime(const std::string& id);
				inline int memberSize();

			public:
				std::map<std::string, GroupModel::MemberInfo> memberMap;
				std::string creator;
				int status;
		};

	public:
		GroupModel() : _storage(NULL) {}
		virtual ~GroupModel(){
			if(this->_storage != NULL){
				delete this->_storage;
				this->_storage = NULL;
			}	
		}

		bool init();

		bool getGroupInfo(const std::string& groupId, GroupInfo**);
		bool setGroupInfo(const std::string& groupId, GroupInfo*);
		bool delGroupInfo(const std::string& groupId);
	
		static bool legalGroupStatus(int);
	private:	
		
		std::string _serialize(GroupInfo*);
		GroupInfo* _unserialize(const std::string& content);
	
	private:
		GroupStorage* _storage;
	
};

inline void GroupModel::GroupInfo::addMember(const std::string& id){/*{{{*/
	GroupModel::MemberInfo memberInfo;
	memberInfo.activeTime 	= 0;
	memberInfo.joinTime		= g_timeNow;
	this->addMember(id, memberInfo);
}/*}}}*/

inline void GroupModel::GroupInfo::addMember(const std::string& id, const GroupModel::MemberInfo& memberInfo){/*{{{*/
	this->memberMap.insert(std::pair<std::string, GroupModel::MemberInfo>(id, memberInfo));
}/*}}}*/

inline bool GroupModel::GroupInfo::memberExist(const std::string& id){/*{{{*/
	std::map<std::string, GroupModel::MemberInfo>::iterator iter;
	iter = this->memberMap.find(id);
	if(iter == this->memberMap.end()){
		return false;
	} else {
		return true;
	}
}/*}}}*/

inline void GroupModel::GroupInfo::delMember(const std::string& id){/*{{{*/
	this->memberMap.erase(id);
}/*}}}*/

inline void GroupModel::GroupInfo::updateMemberActiveTime(const std::string& id, time_t time){/*{{{*/
	std::map<std::string, GroupModel::MemberInfo>::iterator iter;
	iter = this->memberMap.find(id);
	if(iter == this->memberMap.end()){
		return;
	}
	iter->second.activeTime = time;
}/*}}}*/

inline time_t GroupModel::GroupInfo::memberActiveTime(const std::string& id){/*{{{*/
	std::map<std::string, GroupModel::MemberInfo>::iterator iter;
	iter = this->memberMap.find(id);
	if(iter == this->memberMap.end()){
		return 0;
	} else {
		return iter->second.activeTime;
	}
}/*}}}*/

inline int GroupModel::GroupInfo::memberSize(){/*{{{*/
	return (int)(this->memberMap.size());
}/*}}}*/

#endif
