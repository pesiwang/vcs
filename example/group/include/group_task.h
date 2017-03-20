
#ifndef _GROUP_TASK_H
#define _GROUP_TASK_H

#include "vcs/worker/worker_manager.h"
#include "protobuf/group_package.pb.h"

#define TASK_UNKNOWN		0

#define TASK_CREATE_GROUP		1
#define TASK_DISMISS_GROUP		2
#define TASK_JOIN_GROUP			3
#define TASK_QUIT_GROUP			4
#define TASK_MSG_GROUP			5
#define TASK_REGISTER_CHAT		6
#define TASK_GET_GROUP_INFO		7
#define TASK_SET_GROUP_STATUS	8
#define TASK_BROADCAST_GROUP	9

class TaskUnknown : public vcs::WorkerTask {

};

class TaskCreateGroup : public vcs::WorkerTask {
	public:
		group::RequestCreateGroup requestCreateGroup;
};

class TaskDismissGroup : public vcs::WorkerTask {
	public:
		group::RequestDismissGroup requestDismissGroup;
};

class TaskJoinGroup : public vcs::WorkerTask {
	public:
		group::RequestJoinGroup requestJoinGroup;
};

class TaskQuitGroup : public vcs::WorkerTask {
	public:
		group::RequestQuitGroup requestQuitGroup;
};

class TaskMsgGroup : public vcs::WorkerTask {
	public:
		group::RequestMsgGroup requestMsgGroup;
};

class TaskRegisterChat : public vcs::WorkerTask {

};

class TaskGetGroupInfo : public vcs::WorkerTask {
	public:
		group::RequestGetGroupInfo requestGetGroupInfo;
};

class TaskSetGroupStatus : public vcs::WorkerTask {
	public:
		group::RequestSetGroupStatus requestSetGroupStatus;
};

class TaskBroadcastGroup : public vcs::WorkerTask {
	public:
		group::RequestBroadcastGroup requestBroadcastGroup;
};
#endif

