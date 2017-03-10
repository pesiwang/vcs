#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "event_base/helper.h"
#include "event_base/event_base.h"

using namespace std;
using namespace vcs;

time_t g_timeNow = 0;
time_t g_timeRecent = 0;

EventBase::EventBase() : _status(VCS_STATUS_STOPPED), _epollFd(-1)/*{{{*/
{
	g_timeNow = time(NULL);
	g_timeRecent = g_timeNow;

	this->_epollFd = ::epoll_create(64);
	if(this->_epollFd < 0){
		log_error("cannot create epoll file descriptor");
		exit(EXIT_FAILURE);
	}

	if(!Helper::Application::setSignalHandler(SIGPIPE, SIG_IGN)){
		log_error("failed to set SIGPIPE to SIG_IGN");
		exit(EXIT_FAILURE);
	}
	if(!Helper::Application::setSignalHandler(SIGINT, EventBase::stop)){
		log_error("failed to set SIGINT to handle_signal_int");
		exit(EXIT_FAILURE);
	}
}/*}}}*/

EventBase::~EventBase()/*{{{*/
{
	::close(this->_epollFd);
	this->_epollFd = -1;
}/*}}}*/

EventBase* EventBase::instance()/*{{{*/
{
	static EventBase self;
	return &self;
}/*}}}*/

void EventBase::stop(int sig)/*{{{*/
{
	instance()->_status = VCS_STATUS_STOPPING;
}/*}}}*/

void EventBase::dispatch()/*{{{*/
{
	this->_status = VCS_STATUS_RUNNING;
	for(list<Observer*>::iterator iter = this->_observers.begin(); iter != this->_observers.end(); ++iter)
		(*iter)->onStarted();

	while(VCS_STATUS_RUNNING == this->_status){
		struct epoll_event events[64];
		g_timeNow = time(NULL);
		if(g_timeNow >= g_timeRecent + 1){
			g_timeRecent = g_timeNow;
			this->_doTimer();
		}

		int nfds = epoll_wait(this->_epollFd, events, 64, 1000);
		if (nfds < 0)
			break;

		for(int i = 0; i < nfds; ++i){
			if(events[i].events & EPOLLIN){
				map<int, Listener*>::iterator iter = this->_listeners.find(events[i].data.fd);
				if(this->_listeners.end() != iter){
					Listener* listener = iter->second;
					int clientFd = -1;
					while((clientFd = Helper::Socket::accept(listener->fd)) >= 0){
						listener->onAccepted(clientFd);
					}
				}
				else{
					map<int, Handler*>::iterator iter = this->_handlers.find(events[i].data.fd);
					if(this->_handlers.end() == iter){
						log_error("no handler associated with socket %d", events[i].data.fd);
					}
					else{
						Handler* handler = iter->second;
						handler->lastActiveTime = g_timeNow;

						if(!Helper::Socket::read(handler->fd, handler->input))
							this->remove(handler);
						else if(handler->input->size() > 0)
							handler->onData();
					}
				}
			}
			else if(events[i].events & EPOLLOUT){
				map<int, Handler*>::iterator iter = this->_handlers.find(events[i].data.fd);
				if(this->_handlers.end() == iter){
					log_error("no handler associated with socket %d", events[i].data.fd);
				}
				else{
					Handler* handler = iter->second;
					if(!handler->connected){
						int result;
						socklen_t result_len = sizeof(result);
						if((getsockopt(handler->fd, SOL_SOCKET, SO_ERROR, &result, &result_len) < 0) || (result != 0)){
							log_error("connect failed for socket %d", handler->fd);
							this->remove(handler);
						}
						else{
							handler->connected = true;
							handler->onConnected();
						}
					}
					this->write(handler);
				}
			}
		}
	}

	log_info("event base stopping, begin to clean up");
	for(list<Observer*>::iterator iter = this->_observers.begin(); iter != this->_observers.end(); ++iter)
		(*iter)->onStopping();

	list<Listener*> listeners;
	for(map<int, Listener*>::iterator iter = this->_listeners.begin(); iter != this->_listeners.end(); ++iter)
		listeners.push_back(iter->second);
	for(list<Listener*>::iterator iter = listeners.begin(); iter != listeners.end(); ++iter)
		this->remove(*iter);

	list<Handler*> handlers;
	for(map<int, Handler*>::iterator iter = this->_handlers.begin(); iter != this->_handlers.end(); ++iter)
		handlers.push_back(iter->second);
	for(list<Handler*>::iterator iter = handlers.begin(); iter != handlers.end(); ++iter)
		this->remove(*iter);

	this->_status = VCS_STATUS_STOPPED;
	for(list<Observer*>::iterator iter = this->_observers.begin(); iter != this->_observers.end(); ++iter)
		(*iter)->onStopped();

	log_info("event base stopped");
}/*}}}*/

void EventBase::add(Listener* listener)/*{{{*/
{
	if(NULL != this->_listeners[listener->fd]){
		log_error("cannot add listener for socket %d for an existing one is there already", listener->fd);
		return;
	}

	struct epoll_event ev = {0};
	ev.events = EPOLLIN|EPOLLET;
	ev.data.fd = listener->fd;
	if(epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, listener->fd, &ev) < 0){
		log_error("failed to add fd %d to epoll", listener->fd);
		return;
	}
	this->_listeners[listener->fd] = listener;
}/*}}}*/

void EventBase::add(Handler* handler)/*{{{*/
{
	if(NULL != this->_handlers[handler->fd]){
		log_error("cannot add handler for socket %d for an existing one is there already", handler->fd);
		return;
	}

	struct epoll_event ev = {0};
	ev.events = EPOLLIN|EPOLLOUT|EPOLLET;
    ev.data.fd = handler->fd;
    if(epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, handler->fd, &ev) < 0)
		log_error("failed to add fd %d to epoll", handler->fd);

	this->_handlers[handler->fd] = handler;
}/*}}}*/

void EventBase::add(Observer* observer)/*{{{*/
{
	this->_observers.push_back(observer);
}/*}}}*/

void EventBase::remove(Listener* listener)/*{{{*/
{
	::close(listener->fd);
	map<int, Listener*>::iterator iter = this->_listeners.find(listener->fd);
	if(this->_listeners.end() != iter){
		Listener* listener = iter->second;
		listener->onClosed();
		this->_listeners.erase(iter);
	}
}/*}}}*/

void EventBase::remove(Handler* handler)/*{{{*/
{
	::close(handler->fd);
	map<int, Handler*>::iterator iter = this->_handlers.find(handler->fd);
	if(this->_handlers.end() != iter){
		Handler* handler = iter->second;
		handler->connected = false;
		handler->onClosed();
		this->_handlers.erase(iter);
	}
}/*}}}*/

void EventBase::remove(Observer* observer)/*{{{*/
{
	this->_observers.remove(observer);
}/*}}}*/

void EventBase::write(Handler* handler)/*{{{*/
{
	if(handler->output->size() > 0){
		if(!Helper::Socket::write(handler->fd, handler->output)){
			this->remove(handler);
			return;
		}
	}
}/*}}}*/

EventBase::Handler* EventBase::getHandler(int fd)/*{{{*/
{
	map<int, Handler*>::iterator iter = this->_handlers.find(fd);
	if(this->_handlers.end() == iter)
		return NULL;
	return iter->second;
}/*}}}*/

EventBase::Listener* EventBase::getListener(int fd)/*{{{*/
{
	map<int, Listener*>::iterator iter = this->_listeners.find(fd);
	if(this->_listeners.end() == iter)
		return NULL;
	return iter->second;
}/*}}}*/

size_t EventBase::listenerNum(){
	return this->_listeners.size();
}

size_t EventBase::handlerNum(){
	return this->_handlers.size();
}

size_t EventBase::observerNum(){
	return this->_observers.size();
}

/////////////////////////////////////////////////////////////////

void EventBase::_doTimer()/*{{{*/
{
	list<Handler*> handlers;
	for(map<int, Handler*>::iterator iter = this->_handlers.begin(); iter != this->_handlers.end(); ++iter){
		Handler* handler = iter->second;
		if((handler->maxIdleTime != UNLIMITED_IDLE_TIME) && ((handler->lastActiveTime + handler->maxIdleTime) <= g_timeNow))
			handlers.push_back(handler);
	}

	for(list<Handler*>::iterator iter = handlers.begin(); iter != handlers.end(); ++iter)
		this->remove(*iter);

	for(list<Observer*>::iterator iter = this->_observers.begin(); iter != this->_observers.end(); ++iter)
		(*iter)->onTimer();
}/*}}}*/

