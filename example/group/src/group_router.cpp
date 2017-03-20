#include "group_router.h"
#include "group_task.h"
#include <string>
#include <stdlib.h>

using namespace vcs;
using namespace std;

unsigned int GroupRouter::groupIdToKey(const string& groupId) {/*{{{*/

	int index = groupId.size() - 2;
	if(index <= 0){
		index = 0;
	}
	string s = groupId.substr(index);
	return (unsigned int)strtol(s.c_str(), NULL, 16);
}/*}}}*/

unsigned int GroupRouter::createKey(WorkerTask* task) {/*{{{*/
	switch(task->type){
		case TASK_CREATE_GROUP:
			return GroupRouter::groupIdToKey( ((TaskCreateGroup*)task)->requestCreateGroup.group_id() );

		case TASK_DISMISS_GROUP:
			return GroupRouter::groupIdToKey( ((TaskDismissGroup*)task)->requestDismissGroup.group_id() );

		case TASK_JOIN_GROUP:
			return GroupRouter::groupIdToKey( ((TaskJoinGroup*)task)->requestJoinGroup.group_id() );

		case TASK_QUIT_GROUP:
			return GroupRouter::groupIdToKey( ((TaskQuitGroup*)task)->requestQuitGroup.group_id() );
			
		case TASK_MSG_GROUP:
			return GroupRouter::groupIdToKey( ((TaskMsgGroup*)task)->requestMsgGroup.group_id() );

		case TASK_REGISTER_CHAT:
			return 0;

		case TASK_GET_GROUP_INFO:
			return GroupRouter::groupIdToKey( ((TaskGetGroupInfo*)task)->requestGetGroupInfo.group_id() );

		case TASK_SET_GROUP_STATUS:
			return GroupRouter::groupIdToKey( ((TaskSetGroupStatus*)task)->requestSetGroupStatus.group_id() );

		case TASK_BROADCAST_GROUP:
			return GroupRouter::groupIdToKey( ((TaskBroadcastGroup*)task)->requestBroadcastGroup.group_id() );

		default:
			log_error("unknown task type, task->type=%d", task->type);
			return 0;
	}
}/*}}}*/

