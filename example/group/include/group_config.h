#ifndef _GROUP_CONFIG_H
#define _GROUP_CONFIG_H

#include "vcs/worker/worker_manager.h"

class GroupConfig : public vcs::WorkerConfig {
	public:
		vcs::WorkerRouter* instanceRouter();
		vcs::WorkerCodec* instanceCodec();
		vcs::WorkerService* createService();
		int threadNum();
		int clientMaxIdleTime();
};

#endif

