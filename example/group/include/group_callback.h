#ifndef _GROUP_CALLBACK_H
#define _GROUP_CALLBACK_H

#include "vcs/worker/worker_manager.h"

class ChatClientCallback : public vcs::WorkerClientCallback {
	public:
		void onConnected(vcs::EventBase::Handler* handler);
		void onTask(vcs::EventBase::Handler* handler, vcs::WorkerTask* task);
		void onClosed(vcs::EventBase::Handler* handler);
};

#endif
