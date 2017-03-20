#include "vcs/event_base/event_base.h"
#include "vcs/util/server_config.h"
#include "model/group_model.h"
#include "model/group_storage.h"
#include "json/json.h"

using namespace std;
using namespace vcs;

bool GroupModel::init(){/*{{{*/
	if(this->_storage != NULL && this->_storage->isConnected()){
		return true;
	}
	
	if(this->_storage != NULL){
		// not connected any more, delete it
		delete this->_storage;
		this->_storage	= NULL;
	}

	this->_storage	= new GroupStorage();
	ServerConfig* serverConfig = Singleton<ServerConfig>::instance();
	this->_storage->host	= serverConfig->getStrValue("storage.host");
	this->_storage->port	= serverConfig->getIntValue("storage.port");
	if( ! this->_storage->connect()){
		log_error("connect to storage(%s:%d) fail", this->_storage->host.c_str(), this->_storage->port); 
		return false;
	}

	return true;
}/*}}}*/

bool GroupModel::getGroupInfo(const string& groupId, GroupInfo** infoAddr) {/*{{{*/
	if(groupId.size() <= 0){
		log_error("groupId is empty");
		return false;
	}

	if(infoAddr == NULL){
		log_error("infoAddr is NULL");
		return false;
	}

	if( ! this->_storage){
		log_error("storage not init"); 
		return false;
	}
	
	string content = "";
	if( ! this->_storage->get(GROUP_HASH_KEY, groupId, content)){
		log_error("storage get fail");
		return false;
	}

	if(content.size() <= 0){
		*infoAddr	= NULL;
		return true;
	}

	*infoAddr = this->_unserialize(content);
	if(*infoAddr == NULL){
		log_error("_unserialize content fail, content=[%s]", content.c_str());
		return false;
	}

	return true;	
}/*}}}*/

bool GroupModel::setGroupInfo(const string& groupId, GroupInfo* info) {/*{{{*/
	if(groupId.size() <= 0){
		log_error("groupId is empty");
		return false;
	}

	if(info == NULL){
		log_error("info is NULL");
		return false;
	}

	if( ! this->_storage){
		log_error("storage not init"); 
		return false;
	}

	string content = this->_serialize(info);
	if(content.size() <= 0){
		log_error("_serialize fail, _serialized content is empty");	
		return false;
	}

	if( ! this->_storage->set(GROUP_HASH_KEY, groupId, content)){
		log_error("storage->set fail,groupId=%s", groupId.c_str());
		return false;
	}

	return true;	
}/*}}}*/

bool GroupModel::delGroupInfo(const string& groupId) {/*{{{*/
	if(groupId.size() <= 0){
		log_error("groupId is empty");
		return false;
	}

	if( ! this->_storage){
		log_error("storage not init"); 
		return false;
	}

	string content = "";
	if( ! this->_storage->get(GROUP_HASH_KEY, groupId, content)){
		log_error("storage get fail");
		return false;
	}

	if( ! this->_storage->set(GROUP_DELETED_HASH_KEY, groupId, content)){
		log_error("storage->set fail,groupId=%s", groupId.c_str());
		return false;
	}

	if( ! this->_storage->del(GROUP_HASH_KEY, groupId)){
		log_error("storage->del fail,groupId=%s", groupId.c_str());
		return false;
	}

	return true;	
}/*}}}*/

string GroupModel::_serialize(GroupInfo* info){/*{{{*/
	Json::Value data;
	data["creator"]	= info->creator;
	data["status"]	= info->status;

	Json::Value memberArr;
	map<string, GroupModel::MemberInfo>::iterator iter;
	for(iter = info->memberMap.begin(); iter != info->memberMap.end(); ++iter){
		Json::Value m;
		m["id"] = iter->first;
		m["activeTime"] = (unsigned int)(iter->second.activeTime);
		m["joinTime"] = (unsigned int)(iter->second.joinTime);
		memberArr.append(m);
	}
	data["member"] = memberArr;

	Json::FastWriter writer;
	return writer.write(data);
}/*}}}*/

GroupModel::GroupInfo* GroupModel::_unserialize(const string& content){/*{{{*/
	Json::Reader reader;
	Json::Value data;
	if( ! reader.parse(content, data)){
		log_error("parse content fail, content=[%s]", content.c_str());	
		return NULL;
	}
	
	GroupModel::GroupInfo* info = new GroupInfo();
	info->creator			= data["creator"].asString();
	info->status			= data["status"].asInt();
	Json::Value memberArr	= data["member"];
	int memberSize 			= memberArr.size();
	for(int i = 0; i < memberSize; ++i){
		string memberId = memberArr[i]["id"].asString();
		GroupModel::MemberInfo memberInfo;
		memberInfo.activeTime = memberArr[i]["activeTime"].asUInt();
		memberInfo.joinTime = memberArr[i]["joinTime"].asUInt();
		info->memberMap.insert(pair<string, GroupModel::MemberInfo>(memberId, memberInfo));
	}

	return info;
}/*}}}*/

bool GroupModel::legalGroupStatus(int status){/*{{{*/
	if(status == GroupModel::GROUP_STATUS_NORMAL 
			|| status == GroupModel::GROUP_STATUS_GRAY
			|| status == GroupModel::GROUP_STATUS_BLACK)
	{
		return true;
	} else {
		return false;
	}
}/*}}}*/

