#include "vcs/util/vc_codec.h"
#include "group_codec.h"
#include "group_task.h"
#include "group_outcome.h"
#include <string>
#include <arpa/inet.h>

using namespace vcs;
using namespace std;

WorkerTask* GroupCodec::decode(Buffer* buffer){/*{{{*/
	VCCodec::Package* p	= VCCodec::decode(buffer);
	if(p->status == VCCodec::PACKAGE_STATUS_PARTIAL){
		log_debug("not enough data to decode");
		delete p;
		return NULL;
	} else if(p->status == VCCodec::PACKAGE_STATUS_CORRUPT){
		log_error("decode fail, data corrupt");
		WorkerTask* task = new TaskUnknown();
		task->type	= TASK_UNKNOWN;
		task->corrupted	= true;
		delete p;
		return task;
	}

	WorkerTask* task = this->_createTask(p->head.cmd);	
	if(p->head.version != GROUP_VERSION){
		log_error("incorrect head version = %d", p->head.version);
		task->corrupted	= true;
		delete p;
		return task;
	}		
	
	if(task->type == TASK_UNKNOWN){
		log_error("unknown cmd, cmd = %d", p->head.cmd);
		task->corrupted = true;
		delete p;
		return task;
	}

	this->_parseBody(task, p->body);

	delete p;
	return task;
}/*}}}*/

Buffer* GroupCodec::encode(WorkerOutcome* outcome){/*{{{*/
	string* body = new string();
	unsigned char cmd;
	switch(outcome->type){
		case OUTCOME_CREATE_GROUP:
			if( ! ((OutcomeCreateGroup*)outcome)->responseCreateGroup.SerializeToString(body)){
				log_error("responseCreateGroup.SerializeToString fail");
			}
			cmd	= CMD_RESP_CREATE_GROUP;
			break;

		case OUTCOME_DISMISS_GROUP:
			if( ! ((OutcomeDismissGroup*)outcome)->responseDismissGroup.SerializeToString(body)){
				log_error("responseDismissGroup.SerializeToString fail");
			}
			cmd	= CMD_RESP_DISMISS_GROUP;
			break;

		case OUTCOME_JOIN_GROUP:
			if( ! ((OutcomeJoinGroup*)outcome)->responseJoinGroup.SerializeToString(body)){
				log_error("responseJoinGroup.SerializeToString fail");
			}
			cmd	= CMD_RESP_JOIN_GROUP;
			break;

		case OUTCOME_QUIT_GROUP:
			if( ! ((OutcomeQuitGroup*)outcome)->responseQuitGroup.SerializeToString(body)){
				log_error("responseQuitGroup.SerializeToString fail");
			}
			cmd	= CMD_RESP_QUIT_GROUP;
			break;
		
		case OUTCOME_MSG_GROUP:
			if( ! ((OutcomeMsgGroup*)outcome)->responseMsgGroup.SerializeToString(body)){
				log_error("responseMsgGroup.SerializeToString fail");
			}
			cmd	= CMD_RESP_MSG_GROUP;
			break;

		case OUTCOME_REGISTER_CHAT:
			if( ! ((OutcomeRegisterChat*)outcome)->responseRegisterChat.SerializeToString(body)){
				log_error("responseRegisterChat.SerializeToString fail");
			}
			cmd	= CMD_RESP_REGISTER_CHAT;
			break;

		case OUTCOME_GET_GROUP_INFO:
			if( ! ((OutcomeGetGroupInfo*)outcome)->responseGetGroupInfo.SerializeToString(body)){
				log_error("responseGetGroupInfo.SerializeToString fail");
			}
			cmd	= CMD_RESP_GET_GROUP_INFO;
			break;

		case OUTCOME_SET_GROUP_STATUS:
			if( ! ((OutcomeSetGroupStatus*)outcome)->responseSetGroupStatus.SerializeToString(body)){
				log_error("responseSetGroupStatus.SerializeToString fail");
			}
			cmd	= CMD_RESP_SET_GROUP_STATUS;
			break;

		case OUTCOME_BROADCAST_GROUP:
			if( ! ((OutcomeBroadcastGroup*)outcome)->responseBroadcastGroup.SerializeToString(body)){
				log_error("responseBroadcastGroup.SerializeToString fail");
			}
			cmd	= CMD_RESP_BROADCAST_GROUP;
			break;

		default:
			delete body;
			log_error("unknown outcome type");
			return NULL;
	}
	
	Buffer* buffer = VCCodec::encode(GROUP_VERSION, cmd, *body);	
	
	delete body;
	return buffer;
}/*}}}*/


WorkerTask* GroupCodec::_createTask(unsigned char cmd){/*{{{*/
	WorkerTask* task;
	switch(cmd){
		case CMD_CREATE_GROUP:
			task		= new TaskCreateGroup();
			task->type	= TASK_CREATE_GROUP; 
			return task;

		case CMD_DISMISS_GROUP:
			task		= new TaskDismissGroup();
			task->type	= TASK_DISMISS_GROUP; 
			return task;

		case CMD_JOIN_GROUP:
			task		= new TaskJoinGroup();
			task->type	= TASK_JOIN_GROUP; 
			return task;

		case CMD_QUIT_GROUP:
			task		= new TaskQuitGroup();
			task->type	= TASK_QUIT_GROUP;
			return task;

		case CMD_MSG_GROUP:
			task		= new TaskMsgGroup();
			task->type	= TASK_MSG_GROUP;
			return task;

		case CMD_REGISTER_CHAT:
			task		= new TaskRegisterChat();
			task->type	= TASK_REGISTER_CHAT;
			return task;

		case CMD_GET_GROUP_INFO:
			task		= new TaskGetGroupInfo();
			task->type	= TASK_GET_GROUP_INFO;
			return task;

		case CMD_SET_GROUP_STATUS:
			task		= new TaskSetGroupStatus();
			task->type	= TASK_SET_GROUP_STATUS;
			return task;

		case CMD_BROADCAST_GROUP:
			task		= new TaskBroadcastGroup();
			task->type	= TASK_BROADCAST_GROUP;
			return task;

		default:
			task		= new TaskUnknown();
			task->type	= TASK_UNKNOWN;
			return task;
	}
}/*}}}*/

void GroupCodec::_parseBody(WorkerTask* task, const string& body){/*{{{*/
	switch(task->type){
		case TASK_CREATE_GROUP:
			if( ! ((TaskCreateGroup*)task)->requestCreateGroup.ParseFromString(body)){
				task->corrupted = true;	
			}
			break;

		case TASK_DISMISS_GROUP:
			if( ! ((TaskDismissGroup*)task)->requestDismissGroup.ParseFromString(body)){
				task->corrupted = true;
			}
			break;

		case TASK_JOIN_GROUP:
			if( ! ((TaskJoinGroup*)task)->requestJoinGroup.ParseFromString(body)){
				task->corrupted = true;
			}
			break;

		case TASK_QUIT_GROUP:
			if( ! ((TaskQuitGroup*)task)->requestQuitGroup.ParseFromString(body)){
				task->corrupted = true;
			}
			break;

		case TASK_MSG_GROUP:
			if( ! ((TaskMsgGroup*)task)->requestMsgGroup.ParseFromString(body)){
				task->corrupted = true;
			}
			break;

		case TASK_REGISTER_CHAT:
			break;

		case TASK_GET_GROUP_INFO:
			if( ! ((TaskGetGroupInfo*)task)->requestGetGroupInfo.ParseFromString(body)){
				task->corrupted = true;
			}
			break;

		case TASK_SET_GROUP_STATUS:
			if( ! ((TaskSetGroupStatus*)task)->requestSetGroupStatus.ParseFromString(body)){
				task->corrupted = true;
			}
			break;

		case TASK_BROADCAST_GROUP:
			if( ! ((TaskBroadcastGroup*)task)->requestBroadcastGroup.ParseFromString(body)){
				task->corrupted = true;
			}
			break;

		default:
			task->corrupted = true;
			break;
	}
}/*}}}*/

