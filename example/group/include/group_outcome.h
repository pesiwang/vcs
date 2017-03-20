

#ifndef _GROUP_OUTCOME_H
#define _GROUP_OUTCOME_H
#include "vcs/worker/worker_manager.h"

#define OUTCOME_CREATE_GROUP		1
#define OUTCOME_DISMISS_GROUP		2
#define OUTCOME_JOIN_GROUP			3
#define OUTCOME_QUIT_GROUP			4
#define OUTCOME_MSG_GROUP			5
#define OUTCOME_REGISTER_CHAT		6	
#define OUTCOME_GET_GROUP_INFO		7
#define OUTCOME_SET_GROUP_STATUS	8
#define OUTCOME_BROADCAST_GROUP		9

class OutcomeCreateGroup : public vcs::WorkerOutcome {
	public:
		group::ResponseCreateGroup responseCreateGroup;
};

class OutcomeDismissGroup : public vcs::WorkerOutcome {
	public:
		group::ResponseDismissGroup responseDismissGroup;
};

class OutcomeJoinGroup : public vcs::WorkerOutcome {
	public:
		group::ResponseJoinGroup responseJoinGroup;
};

class OutcomeQuitGroup : public vcs::WorkerOutcome {
	public:
		group::ResponseQuitGroup responseQuitGroup;
};

class OutcomeMsgGroup : public vcs::WorkerOutcome {
	public:
		group::ResponseMsgGroup responseMsgGroup;
};

class OutcomeRegisterChat : public vcs::WorkerOutcome {
	public:
		group::ResponseRegisterChat responseRegisterChat;
};

class OutcomeGetGroupInfo : public vcs::WorkerOutcome {
	public:
		group::ResponseGetGroupInfo responseGetGroupInfo;
};

class OutcomeSetGroupStatus : public vcs::WorkerOutcome {
	public:
		group::ResponseSetGroupStatus responseSetGroupStatus;
};

class OutcomeBroadcastGroup : public vcs::WorkerOutcome {
	public:
		group::ResponseBroadcastGroup responseBroadcastGroup;
};

#endif

