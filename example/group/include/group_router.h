#ifndef _GROUP_ROUTER_H
#define _GROUP_ROUTER_H

#include <string>
#include "vcs/worker/worker_manager.h"

class GroupRouter : public vcs::WorkerRouter {
	public:
		unsigned int createKey(vcs::WorkerTask*);
		static unsigned int groupIdToKey(const std::string& groupId);
};


#endif
