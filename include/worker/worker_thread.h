#ifndef _VCS_WORKER_THREAD_H
#define _VCS_WORKER_THREAD_H

#include <queue>
#include <pthread.h>
#include "worker_base.h"

namespace vcs {
	enum THREAD_STATUS {
		THREAD_STATUS_STOPPED = 0,
		THREAD_STATUS_RUNNING,
		THREAD_STATUS_STOPPING
	};

	class WorkerThread {

		public:
			WorkerThread(int threadIndex, WorkerService* service,
					 unsigned int taskQueueSizeThreshold,
					 unsigned int outcomeQueueSizeThreshold);

			virtual ~WorkerThread();
			static void* threadEntry(void*);

			void create();
			void stop();
			void pushAndNotify(WorkerTask*);
			WorkerTask* popTask();
			void pushAndNotify(WorkerOutcome*);
			WorkerOutcome* popOutcome();
			
			size_t taskQueueSize();
			size_t outcomeQueueSize();

		public:
			THREAD_STATUS	status;
			int threadIndex;
			WorkerService*	service;

			pthread_t threadId;
			int taskReadFd;
			int taskWriteFd;
			int outcomeReadFd;
			int outcomeWriteFd;

		private:
			void _init();
			void _doWork();
			void _check();

			pthread_mutex_t			_taskQueueMutex; 
			std::queue<WorkerTask*> _taskQueue;

			pthread_mutex_t				_outcomeQueueMutex;
			std::queue<WorkerOutcome*>	_outcomeQueue;

			unsigned int _taskQueueSizeThreshold;
			unsigned int _outcomeQueueSizeThreshold;

	};
	
}

#endif
