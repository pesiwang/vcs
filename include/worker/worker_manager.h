#ifndef _VCS_WORKER_MANAGER_H
#define _VCS_WORKER_MANAGER_H

#include <vector>
#include "worker_base.h"
#include "worker_manager.h"
#include "worker_event.h"
#include "worker_thread.h"

#define EACH_READ_NUM		10
#define DEFAULT_TASK_QUEUE_SIZE_THRESHOLD			3
#define DEFAULT_OUTCOME_QUEUE_SIZE_THRESHOLD		3

namespace vcs {
	class WorkerManager {

		public:
			static WorkerManager* instance();

		public:
			void init(const char* host, unsigned short port, WorkerConfig* config, 
					unsigned int taskQueueSizeThreshold = DEFAULT_TASK_QUEUE_SIZE_THRESHOLD, 
					unsigned int outcomeQueueSizeThreshold = DEFAULT_OUTCOME_QUEUE_SIZE_THRESHOLD);

			void stop();
			void routeTask(WorkerTask*);
			WorkerTask* decode(Buffer*);
			Buffer* encode(WorkerOutcome*);

		public:
			int threadNum();
			size_t taskQueueSize(size_t);
			size_t outcomeQueueSize(size_t);

			void addClientCallback(WorkerClientCallback* callback);
			void removeClientCallback(WorkerClientCallback* callback);
			void onClientConnected(EventBase::Handler* handler);
			void onClientTask(EventBase::Handler* handler, WorkerTask* task);
			void onClientClosed(EventBase::Handler* handler);
	
		private:
			WorkerManager();
			virtual ~WorkerManager();
			void _addWorkerOutcomeHandler(WorkerThread*);
			void _createThreadGroup();

		private:
			WorkerConfig* _config;
			WorkerCodec* _codec;
			WorkerRouter* _router;

			int _threadNum;
			std::vector<WorkerThread*> _threadVector;

			unsigned int _taskQueueSizeThreshold;
			unsigned int _outcomeQueueSizeThreshold;

			std::list<WorkerClientCallback*> _clientCallbackList;
	};
}

#endif
