#ifndef _GROUP_SERVICE_H
#define _GROUP_SERVICE_H


#include "vcs/worker/worker_manager.h"
#include "model/group_model.h"
#include "group_task.h"
#include "group_outcome.h"

class GroupService : public vcs::WorkerService {
	public:
		GroupService();
		virtual ~GroupService();
		vcs::WorkerOutcome* run(vcs::WorkerTask*);


	public:
		GroupModel* model;

	private:
		vcs::WorkerOutcome* _createGroup(TaskCreateGroup*);
		vcs::WorkerOutcome* _dismissGroup(TaskDismissGroup*);
		vcs::WorkerOutcome* _joinGroup(TaskJoinGroup*);
		vcs::WorkerOutcome* _quitGroup(TaskQuitGroup*);
		vcs::WorkerOutcome* _msgGroup(TaskMsgGroup*);
		vcs::WorkerOutcome* _registerChat(TaskRegisterChat*);
		vcs::WorkerOutcome* _getGroupInfo(TaskGetGroupInfo*);
		vcs::WorkerOutcome* _setGroupStatus(TaskSetGroupStatus*);
		vcs::WorkerOutcome* _broadcastGroup(TaskBroadcastGroup*);

		bool __syncToChat(int, OutcomeMsgGroup*);
		bool __syncToChat(int, OutcomeBroadcastGroup*);
		void __count(const std::string& key);
};


#endif
