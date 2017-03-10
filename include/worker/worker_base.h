#ifndef _VCS_WORKER_BASE_H
#define _VCS_WORKER_BASE_H

#include "event_base/event_base.h"

namespace vcs {

	class WorkerTask {
		public:
			WorkerTask() : corrupted(false) {}
			virtual ~WorkerTask(){}

		public: 
			bool corrupted;
			int handlerId;
			int clientFd;

			int type;
	};

	class WorkerOutcome {
		public:
			WorkerOutcome(){}
			virtual ~WorkerOutcome(){}

		public:
			int handlerId;
			int clientFd;

			int type;
	};

	class WorkerRouter {
		public:
			WorkerRouter(){}
			virtual ~WorkerRouter(){}

		public:
			virtual unsigned int createKey(WorkerTask*) = 0;
	};

	class WorkerCodec {
		public:
			WorkerCodec(){}
			virtual ~WorkerCodec(){}

		public:
			virtual WorkerTask* decode(Buffer*) = 0;
			virtual Buffer* encode(WorkerOutcome*) = 0;
	};

	class WorkerThread;
	class WorkerService {
		public:
			WorkerThread* thread;

		public:
			WorkerService() : thread(NULL){}
			virtual ~WorkerService(){}
			void write(int handlerId, int clientFd, WorkerOutcome* outcome);

			virtual WorkerOutcome* run(WorkerTask*) = 0;

	};

	class WorkerConfig {
		public:
			virtual WorkerRouter* instanceRouter() = 0;
			virtual WorkerCodec* instanceCodec() = 0;
			virtual WorkerService* createService() = 0;
			virtual int threadNum() = 0;
			virtual int clientMaxIdleTime() = 0;
	};

	class WorkerClientCallback {
		public:
			virtual void onConnected(EventBase::Handler* handler) = 0;
			virtual void onTask(EventBase::Handler* handler, WorkerTask* task) = 0;
			virtual void onClosed(EventBase::Handler* handler) = 0;
	};
}

#endif
