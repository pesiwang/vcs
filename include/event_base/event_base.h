#ifndef _VCS_EVENT_BASE_H
#define _VCS_EVENT_BASE_H

#include <map>
#include <list>
#include <time.h>
#include <stdio.h>
#include "buffer.h"
#include "helper.h"
#include "log.h"

#define UNLIMITED_IDLE_TIME		0

extern time_t g_timeNow;

namespace vcs
{
	enum VCS_STATUS
	{
		VCS_STATUS_STOPPED = 0,
		VCS_STATUS_RUNNING,
		VCS_STATUS_STOPPING
	};

	class EventBase
	{
		public:
			class Observer
			{
				public:
					Observer() {}
					virtual ~Observer() {}

					virtual void onStarted() = 0;
					virtual void onStopping() = 0;
					virtual void onStopped() = 0;
					virtual void onTimer() = 0;
			};

			class Listener
			{
				public:
					Listener() : fd(-1) {}
					virtual ~Listener() {}

					virtual void onAccepted(int clientFd) = 0;
					virtual void onClosed() = 0;

				public:
					int fd;
			};

			class Handler
			{
				public:
					Handler() : id(0), fd(-1), input(NULL), output(NULL), 
								connected(false), lastActiveTime(g_timeNow), maxIdleTime(UNLIMITED_IDLE_TIME) 
					{
						this->input = new Buffer(); 
						this->output = new Buffer(); 
					}
					virtual ~Handler() { 
						if(NULL != this->input) { 
							delete this->input; 
							this->input = NULL; 
						} 
						if(NULL != this->output) { 
							delete this->output;
							this->output = NULL; 
						} 
					}

					virtual void onConnected() = 0;
					virtual void onData() = 0;
					virtual void onClosed() = 0;

				public:
					int id;
					int fd;
					Buffer* input;
					Buffer* output;
					bool connected;
					time_t lastActiveTime;
					time_t maxIdleTime;
			};

		public:
			static EventBase* instance();
			static void stop(int sig);
			void dispatch();

			void add(Handler* handler);
			void add(Listener* listener);
			void add(Observer* observer);
			void remove(Handler* handler);
			void remove(Listener* listener);
			void remove(Observer* observer);
			void write(Handler* handler);

			Handler* getHandler(int fd);
			Listener* getListener(int fd);

			size_t listenerNum();
			size_t handlerNum();
			size_t observerNum();

		protected:
			EventBase();
			virtual ~EventBase();

			void _doTimer();

		protected:
			volatile VCS_STATUS _status;
			int _epollFd;

			std::map<int, Listener*> _listeners;
			std::map<int, Handler*> _handlers;
			std::list<Observer*> _observers;
	};
}

#endif
