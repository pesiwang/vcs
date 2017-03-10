#ifndef _VCS_WORKER_EVENT_H
#define _VCS_WORKER_EVENT_H

#include "event_base/event_base.h"
#include "worker_base.h"
#include "worker_thread.h"

namespace vcs {
	class WorkerListener : public EventBase::Listener {
		public:
			WorkerListener() : clientMaxIdleTime(0){}
			void onAccepted(int clientFd);
			void onClosed();
			int clientMaxIdleTime;
	};

	class WorkerClientHandler : public EventBase::Handler {
		public:
			void onConnected();
			void onData();
			void onClosed();
	};

	class WorkerOutcomeHandler : public EventBase::Handler {
		public:
			WorkerThread* thread;

		public:
			void onConnected();
			void onData();
			void onClosed();

		private:
			void encodeAndSendOutcome();

	};

	class WorkerObserver : public EventBase::Observer {
		public:
			void onStarted();
			void onStopping();
			void onStopped();
			void onTimer();
	};
}

#endif
