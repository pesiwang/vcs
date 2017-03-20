#include "group_service.h"
#include "group_task.h"
#include "group_outcome.h"
#include "group_router.h"
#include "chat_manager.h"
#include "model/group_model.h"
#include "vcs/event_base/event_base.h"
#include "vcs/util/server_config.h"
#include "vcs/doctor/server_doctor.h"

using namespace vcs;
using namespace std;

GroupService::GroupService(){
	this->model = new GroupModel();
	this->model->init();
}

GroupService::~GroupService(){
	delete this->model;
}

WorkerOutcome* GroupService::run(WorkerTask* task){/*{{{*/
	switch(task->type){
		case TASK_CREATE_GROUP:
			return this->_createGroup( (TaskCreateGroup*)task );

		case TASK_DISMISS_GROUP:
			return this->_dismissGroup( (TaskDismissGroup*)task );

		case TASK_JOIN_GROUP:
			return this->_joinGroup( (TaskJoinGroup*)task );

		case TASK_QUIT_GROUP:
			return this->_quitGroup( (TaskQuitGroup*)task );
			
		case TASK_MSG_GROUP:
			return this->_msgGroup( (TaskMsgGroup*)task );

		case TASK_REGISTER_CHAT:
			return this->_registerChat( (TaskRegisterChat*)task );

		case TASK_GET_GROUP_INFO:
			return this->_getGroupInfo( (TaskGetGroupInfo*)task );

		case TASK_SET_GROUP_STATUS:
			return this->_setGroupStatus( (TaskSetGroupStatus*)task );

		case TASK_BROADCAST_GROUP:
			return this->_broadcastGroup( (TaskBroadcastGroup*)task );

		default:
			log_error("unknown task type, task->type=%d", task->type);
			return NULL;
	}
}/*}}}*/

WorkerOutcome* GroupService::_createGroup(TaskCreateGroup* task){/*{{{*/
	log_debug("begin to create group");

	OutcomeCreateGroup* outcome = new OutcomeCreateGroup();
	outcome->type = OUTCOME_CREATE_GROUP;
	string groupId	= task->requestCreateGroup.group_id();
	string userId	= task->requestCreateGroup.user_id();
	GroupModel::GroupInfo* info	= NULL;

	if( ! this->model->init()){
		log_error("init model fail");
		outcome->responseCreateGroup.set_result(1);
		goto create_group_err;
	}

	if( ! this->model->getGroupInfo(groupId, &info)){
		log_error("model->getGroupInfo fail");
		outcome->responseCreateGroup.set_result(2);
		goto create_group_err;
	}

	if(info != NULL){
		log_error("group %s existed, can not recreate it", groupId.c_str());
		outcome->responseCreateGroup.set_result(3);
		goto create_group_err;
	}

	info	= new GroupModel::GroupInfo();
	info->creator	= userId;
	info->status	= GroupModel::GROUP_STATUS_NORMAL;
	info->addMember(userId);

	if( ! this->model->setGroupInfo(groupId, info)){
		log_error("model->setGroupInfo fail");
		goto create_group_err;
	}

	outcome->responseCreateGroup.set_result(0);

create_group_err:
	if(info != NULL){
		delete info;
	}
	log_debug("create group done");
	this->__count("create_group");
	return outcome;
}/*}}}*/

WorkerOutcome* GroupService::_dismissGroup(TaskDismissGroup* task){/*{{{*/
	log_debug("begin to dismiss group");
	OutcomeDismissGroup* outcome = new OutcomeDismissGroup();
	outcome->type = OUTCOME_DISMISS_GROUP;

	string groupId	= task->requestDismissGroup.group_id();
	string userId	= task->requestDismissGroup.user_id();
	GroupModel::GroupInfo* info	= NULL;

	if( ! this->model->init()){
		log_error("init model fail");
		outcome->responseDismissGroup.set_result(1);
		goto dismiss_group_err;
	}

	if( ! this->model->getGroupInfo(groupId, &info)){
		log_error("model->getGroupInfo fail");
		outcome->responseDismissGroup.set_result(2);
		goto dismiss_group_err;
	}

	if(info == NULL){
		log_error("group %s not existed, can not dismiss it", groupId.c_str());
		outcome->responseDismissGroup.set_result(3);
		goto dismiss_group_err;
	}

	if(userId.compare(info->creator) != 0){
		log_error("%s not the creator, can not dismiss group %s", userId.c_str(), groupId.c_str());
		outcome->responseDismissGroup.set_result(4);
		goto dismiss_group_err;
	}

	if( ! this->model->delGroupInfo(groupId)){
		log_error("model->delGroupInfo fail");
		outcome->responseDismissGroup.set_result(5);
		goto dismiss_group_err;
	}

	outcome->responseDismissGroup.set_result(0);

dismiss_group_err:
	if(info != NULL){
		delete info;
	}
	log_debug("dismiss group done");
	this->__count("dismiss_group");

	return outcome;
}
/*}}}*/

WorkerOutcome* GroupService::_joinGroup(TaskJoinGroup* task){/*{{{*/
	CALC_TIME_DECLARE;
	CALC_TIME_BEGIN;

	log_debug("begin to join group");

	OutcomeJoinGroup* outcome = new OutcomeJoinGroup();
	outcome->type = OUTCOME_JOIN_GROUP;
	string groupId	= task->requestJoinGroup.group_id();
	string userId	= task->requestJoinGroup.user_id();
	GroupModel::GroupInfo* info	= NULL;
	int maxMemberNum = 0;

	if( ! this->model->init()){
		log_error("init model fail");
		outcome->responseJoinGroup.set_result(1);
		goto join_group_err;
	}

	if( ! this->model->getGroupInfo(groupId, &info)){
		log_error("model->getGroupInfo fail");
		outcome->responseJoinGroup.set_result(2);
		goto join_group_err;
	}

	if(info == NULL){
		log_error("group %s not existed, can not join it", groupId.c_str());
		outcome->responseJoinGroup.set_result(3);
		goto join_group_err;
	}

	if(info->memberExist(userId)){
		log_error("user %s already in group %s, can not join it again",userId.c_str(), groupId.c_str());
		outcome->responseJoinGroup.set_result(4);
		goto join_group_err;
	}

	info->addMember(userId);
	maxMemberNum = Singleton<ServerConfig>::instance()->getIntValue("logic.max_group_member_num");
	if(info->memberSize() > maxMemberNum){
		log_error("max group member num exceed, maxMemberNum:%d, now:%d", maxMemberNum, info->memberSize());
		outcome->responseJoinGroup.set_result(5);
		goto join_group_err;
	}

	if( ! this->model->setGroupInfo(groupId, info)){
		log_error("model->setGroupInfo fail");
		outcome->responseJoinGroup.set_result(6);
		goto join_group_err;
	}

	outcome->responseJoinGroup.set_result(0);

join_group_err:
	if(info != NULL){
		delete info;
	}
	log_debug("join group done");
	this->__count("join_group");
	CALC_TIME_END("join_group");
	return outcome;
}/*}}}*/

WorkerOutcome* GroupService::_quitGroup(TaskQuitGroup* task){/*{{{*/
	log_debug("begin to quit group");

	OutcomeQuitGroup* outcome = new OutcomeQuitGroup();
	outcome->type = OUTCOME_QUIT_GROUP;
	string groupId	= task->requestQuitGroup.group_id();
	string userId	= task->requestQuitGroup.user_id();
	GroupModel::GroupInfo* info	= NULL;

	if( ! this->model->init()){
		log_error("init model fail");
		outcome->responseQuitGroup.set_result(1);
		goto quit_group_err;
	}

	if( ! this->model->getGroupInfo(groupId, &info)){
		log_error("model->getGroupInfo fail");
		outcome->responseQuitGroup.set_result(2);
		goto quit_group_err;
	}

	if(info == NULL){
		log_error("group %s not existed, quit group fail", groupId.c_str());
		outcome->responseQuitGroup.set_result(3);
		goto quit_group_err;
	}

	if( ! info->memberExist(userId)){
		log_error("user %s not in group %s, quit fail", userId.c_str(), groupId.c_str());
		outcome->responseQuitGroup.set_result(4);
		goto quit_group_err;
		
	}

	info->delMember(userId);
	if( ! this->model->setGroupInfo(groupId, info)){
		log_error("model->setGroupInfo fail");
		outcome->responseQuitGroup.set_result(5);
		goto quit_group_err;
	}

	outcome->responseQuitGroup.set_result(0);

quit_group_err:
	if(info != NULL){
		delete info;
	}
	log_debug("quit group done");
	this->__count("quit_group");
	return outcome;
}/*}}}*/

WorkerOutcome* GroupService::_msgGroup(TaskMsgGroup* task){/*{{{*/
	log_debug("begin to msg group");
	OutcomeMsgGroup* outcome = new OutcomeMsgGroup();
	outcome->type = OUTCOME_MSG_GROUP;

	string groupId	= task->requestMsgGroup.group_id();
	string senderId	= task->requestMsgGroup.sender_id();
	string mid		= task->requestMsgGroup.mid();
	int type		= task->requestMsgGroup.type();
	string content	= task->requestMsgGroup.content();
	unsigned long long msgTime	= task->requestMsgGroup.time();

	GroupModel::GroupInfo* info	= NULL;
	group::RequestMsgGroup* requestMsgGroup = NULL;

	map<string, GroupModel::MemberInfo>::iterator iter;
	int lag = 0;

	if( ! this->model->init()){
		log_error("init model fail");
		outcome->responseMsgGroup.set_result(1);
		goto msg_group_err;
	}

	if( ! this->model->getGroupInfo(groupId, &info)){
		log_error("model->getGroupInfo fail");
		outcome->responseMsgGroup.set_result(2);
		goto msg_group_err;
	}

	if(info == NULL){
		log_warn("group %s not existed, msg group fail", groupId.c_str());
		outcome->responseMsgGroup.set_result(3);
		goto msg_group_err;
	}

	if( ! info->memberExist(senderId)){
		log_warn("sender %s not in group %s, msg fail", senderId.c_str(), groupId.c_str());
		outcome->responseMsgGroup.set_result(4);
		goto msg_group_err;
	}

	if(info->status == GroupModel::GROUP_STATUS_BLACK){
		log_warn("group %s status is black, msg fail", groupId.c_str());
		outcome->responseMsgGroup.set_result(5);
		goto msg_group_err;
	}

	lag	= Singleton<ServerConfig>::instance()->getIntValue("logic.update_active_time_lag");
	if(g_timeNow - info->memberActiveTime(senderId) > lag){
		info->updateMemberActiveTime(senderId, g_timeNow);
		if( ! model->setGroupInfo(groupId, info)){
			log_warn("update member active time fail");
		}
	}

	outcome->responseMsgGroup.set_result(0);
	requestMsgGroup = outcome->responseMsgGroup.mutable_request();
	requestMsgGroup->set_sender_id(senderId);
	requestMsgGroup->set_group_id(groupId);
	requestMsgGroup->set_mid(mid);
	requestMsgGroup->set_type(type);
	requestMsgGroup->set_content(content);
	requestMsgGroup->set_time(msgTime);

	for(iter = info->memberMap.begin(); iter != info->memberMap.end(); ++iter){
		if(senderId.compare(iter->first) == 0){
			continue;
		}
		string* recver = outcome->responseMsgGroup.add_recver();
		*recver = iter->first;
	}

	if( ! this->__syncToChat(task->clientFd, outcome)){
		log_error("sync to chat server fail");
		outcome->responseMsgGroup.set_result(5);	
		goto msg_group_err;
	}
	
msg_group_err:
	if(info != NULL){
		delete info;
	}
	log_debug("msg group done");
	this->__count("msg_group");

	return outcome;
}/*}}}*/

WorkerOutcome* GroupService::_registerChat(TaskRegisterChat* task){/*{{{*/
	log_debug("begin to register chat");
	OutcomeRegisterChat* outcome = new OutcomeRegisterChat();
	outcome->type = OUTCOME_REGISTER_CHAT;

	ChatManager::instance()->registerChat(task->clientFd, task->handlerId);
	log_info("register chat fd %d, chat handler id %d", task->clientFd, task->handlerId);
	
	outcome->responseRegisterChat.set_result(0);

	log_debug("register chat done");
	this->__count("register_chat");

	return outcome;
}/*}}}*/

WorkerOutcome* GroupService::_getGroupInfo(TaskGetGroupInfo* task){/*{{{*/
	log_debug("begin to get group info");

	OutcomeGetGroupInfo* outcome = new OutcomeGetGroupInfo();
	outcome->type = OUTCOME_GET_GROUP_INFO;
	string groupId	= task->requestGetGroupInfo.group_id();
	GroupModel::GroupInfo* info	= NULL;
	map<string, GroupModel::MemberInfo>::iterator iter;

	if( ! this->model->init()){
		log_error("init model fail");
		outcome->responseGetGroupInfo.set_result(1);
		goto get_group_info_err;
	}

	if( ! this->model->getGroupInfo(groupId, &info)){
		log_error("model->getGroupInfo fail");
		outcome->responseGetGroupInfo.set_result(2);
		goto get_group_info_err;
	}

	if(info == NULL){
		log_warn("group %s not existed, get group info fail", groupId.c_str());
		outcome->responseGetGroupInfo.set_result(3);
		goto get_group_info_err;
	}

	outcome->responseGetGroupInfo.set_result(0);
	outcome->responseGetGroupInfo.set_group_id(groupId);
	outcome->responseGetGroupInfo.set_creator(info->creator);
	outcome->responseGetGroupInfo.set_status(info->status);

	for(iter = info->memberMap.begin(); iter != info->memberMap.end(); iter++){
		group::GroupMember* member = outcome->responseGetGroupInfo.add_member();
		member->set_id(iter->first);
		member->set_active_time(iter->second.activeTime);
		member->set_join_time(iter->second.joinTime);
	}

get_group_info_err:
	if(info != NULL){
		delete info;
	}
	log_debug("get group info done");
	this->__count("get_group_info");
	return outcome;
}/*}}}*/

WorkerOutcome* GroupService::_setGroupStatus(TaskSetGroupStatus* task){/*{{{*/
	log_debug("begin to set group status");

	OutcomeSetGroupStatus* outcome = new OutcomeSetGroupStatus();
	outcome->type = OUTCOME_SET_GROUP_STATUS;
	string groupId	= task->requestSetGroupStatus.group_id();
	int status		= task->requestSetGroupStatus.status();
	GroupModel::GroupInfo* info	= NULL;

	if( ! this->model->init()){
		log_error("init model fail");
		outcome->responseSetGroupStatus.set_result(1);
		goto set_group_status_err;
	}

	if( ! this->model->getGroupInfo(groupId, &info)){
		log_error("model->getGroupInfo fail");
		outcome->responseSetGroupStatus.set_result(2);
		goto set_group_status_err;
	}

	if(info == NULL){
		log_error("group %s not existed, can not set group status", groupId.c_str());
		outcome->responseSetGroupStatus.set_result(3);
		goto set_group_status_err;
	}
	
	if( ! GroupModel::legalGroupStatus(status)){
		log_error("input group status %d not legal", status);
		outcome->responseSetGroupStatus.set_result(4);
		goto set_group_status_err;
	}

	info->status = status;
	if( ! this->model->setGroupInfo(groupId, info)){
		log_error("model->setGroupInfo fail");
		outcome->responseSetGroupStatus.set_result(6);
		goto set_group_status_err;
	}

	outcome->responseSetGroupStatus.set_result(0);

set_group_status_err:
	if(info != NULL){
		delete info;
	}
	log_debug("set group status done");
	this->__count("set_group_status");
	return outcome;
}/*}}}*/

WorkerOutcome* GroupService::_broadcastGroup(TaskBroadcastGroup* task){/*{{{*/
	log_debug("begin to broadcast group");
	OutcomeBroadcastGroup* outcome = new OutcomeBroadcastGroup();
	outcome->type = OUTCOME_BROADCAST_GROUP;

	string groupId	= task->requestBroadcastGroup.group_id();
	string senderId	= task->requestBroadcastGroup.sender_id();
	string mid		= task->requestBroadcastGroup.mid();
	int type		= task->requestBroadcastGroup.type();
	string content	= task->requestBroadcastGroup.content();
	unsigned long long msgTime	= task->requestBroadcastGroup.time();

	GroupModel::GroupInfo* info	= NULL;
	group::RequestBroadcastGroup* requestBroadcastGroup = NULL;

	map<string, GroupModel::MemberInfo>::iterator iter;

	if( ! this->model->init()){
		log_error("init model fail");
		outcome->responseBroadcastGroup.set_result(1);
		goto broadcast_group_err;
	}

	if( ! this->model->getGroupInfo(groupId, &info)){
		log_error("model->getGroupInfo fail");
		outcome->responseBroadcastGroup.set_result(2);
		goto broadcast_group_err;
	}

	if(info == NULL){
		log_error("group %s not existed, broadcast group fail", groupId.c_str());
		outcome->responseBroadcastGroup.set_result(3);
		goto broadcast_group_err;
	}

	outcome->responseBroadcastGroup.set_result(0);
	requestBroadcastGroup = outcome->responseBroadcastGroup.mutable_request();
	requestBroadcastGroup->set_sender_id(senderId);
	requestBroadcastGroup->set_group_id(groupId);
	requestBroadcastGroup->set_mid(mid);
	requestBroadcastGroup->set_type(type);
	requestBroadcastGroup->set_content(content);
	requestBroadcastGroup->set_time(msgTime);

	for(iter = info->memberMap.begin(); iter != info->memberMap.end(); ++iter){
		string* recver = outcome->responseBroadcastGroup.add_recver();
		*recver = iter->first;
	}

	if( ! this->__syncToChat(task->clientFd, outcome)){
		log_error("sync to chat server fail");
		outcome->responseBroadcastGroup.set_result(4);	
		goto broadcast_group_err;
	}
	
broadcast_group_err:
	if(info != NULL){
		delete info;
	}
	log_debug("broadcast group done");
	this->__count("broadcast_group");

	return outcome;
}/*}}}*/

bool GroupService::__syncToChat(int clientFd, OutcomeBroadcastGroup* obg){/*{{{*/
	OutcomeMsgGroup* omg	= new OutcomeMsgGroup();
	omg->type = OUTCOME_MSG_GROUP;
	omg->responseMsgGroup.set_result(obg->responseBroadcastGroup.result());

	group::RequestBroadcastGroup rbg	= obg->responseBroadcastGroup.request();
	group::RequestMsgGroup* rmg		= omg->responseMsgGroup.mutable_request();
	rmg->set_sender_id(rbg.sender_id());
	rmg->set_group_id(rbg.group_id());
	rmg->set_mid(rbg.mid());
	rmg->set_type(rbg.type());
	rmg->set_content(rbg.content());
	rmg->set_time(rbg.time());

	for(int i = 0; i < obg->responseBroadcastGroup.recver_size(); ++i){
		string* recver	= omg->responseMsgGroup.add_recver();
		*recver	= obg->responseBroadcastGroup.recver(i);
	}

	bool result = this->__syncToChat(clientFd, omg);
	delete omg;

	return result;
}/*}}}*/

bool GroupService::__syncToChat(int clientFd, OutcomeMsgGroup* outcome){/*{{{*/
	if(ChatManager::instance()->isChatFd(clientFd)){
		log_debug("msg group request from chat server, no need to sync");
		return true;
	}

	string groupId		= outcome->responseMsgGroup.request().group_id();
	unsigned int key	= GroupRouter::groupIdToKey(groupId);
	ChatManager::Entity entity = ChatManager::instance()->getChat(key);
	if(entity.chatFd == -1){
		log_error("no chat server connected");
		return false;
	}

	log_debug("sync msg to chat server, groupId=%s, chatFd=%d, chatHandlerId=%d", groupId.c_str(), entity.chatFd, entity.chatHandlerId);
	OutcomeMsgGroup* syncOutcome = new OutcomeMsgGroup(*outcome);
	this->write(entity.chatFd, entity.chatHandlerId, syncOutcome);

	return true;
}/*}}}*/

void GroupService::__count(const string& key){/*{{{*/
	AbilityCounter* counterAbility = (AbilityCounter*)ServerDoctor::instance()->ability("counter");
	if(counterAbility != NULL){
		counterAbility->syncIncr(key);
	}
}/*}}}*/

