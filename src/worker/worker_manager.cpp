#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <list>
#include "event_base/event_base.h"
#include "worker/worker_manager.h"

using namespace vcs;
using namespace std;

WorkerManager::WorkerManager() : _config(NULL), _codec(NULL), _router(NULL){/*{{{*/
	this->_threadNum = 0;
	this->_taskQueueSizeThreshold		= DEFAULT_TASK_QUEUE_SIZE_THRESHOLD;
	this->_outcomeQueueSizeThreshold	= DEFAULT_OUTCOME_QUEUE_SIZE_THRESHOLD;
}/*}}}*/

WorkerManager::~WorkerManager(){/*{{{*/
	std::vector<WorkerThread*>::iterator iter;
	for(iter = _threadVector.begin(); iter != _threadVector.end(); ++iter) {
		delete *iter;
		*iter = NULL;	
	}
}/*}}}*/

WorkerManager* WorkerManager::instance(){/*{{{*/
	static WorkerManager self;
	return &self;
}/*}}}*/

void WorkerManager::init(const char* host, unsigned short port, WorkerConfig* config,
		unsigned int taskQueueSizeThreshold, unsigned int outcomeQueueSizeThreshold)
{/*{{{*/

	this->_taskQueueSizeThreshold		= taskQueueSizeThreshold;
	this->_outcomeQueueSizeThreshold	= outcomeQueueSizeThreshold;

	this->_config		= config;
	this->_threadNum	= this->_config->threadNum();
	this->_router	= this->_config->instanceRouter();
	this->_codec	= this->_config->instanceCodec();

	WorkerListener* listener		= Singleton<WorkerListener>::instance();
	listener->clientMaxIdleTime = this->_config->clientMaxIdleTime();
	listener->fd	= Helper::Socket::listen(host, port);
	if(listener->fd < 0) {
		log_error("listen on %s:%u fail", host, port);
		exit(EXIT_FAILURE);
	}

	EventBase::instance()->add(listener);
	log_info("listen on %s:%u", host, port);

	WorkerObserver* observer = Singleton<WorkerObserver>::instance();
	EventBase::instance()->add(observer);

	this->_createThreadGroup();
}/*}}}*/

void WorkerManager::stop(){/*{{{*/
	for(int i = 0; i < this->_threadNum; ++i){
		this->_threadVector[i]->stop();
	}

	for(int i = 0; i < this->_threadNum; ++i){
		int ret = pthread_join(this->_threadVector[i]->threadId, NULL);
		if(ret == 0){
			log_info("threadVector[%d] stopped", i);
		} else {
			log_error("pthread_join threadVector[%d] fail, errno=%d, error=%s", i, errno, strerror(errno));
		}
	}
}/*}}}*/

void WorkerManager::routeTask(WorkerTask* task){/*{{{*/
	unsigned int routerKey	= this->_router->createKey(task);
	unsigned int index		= routerKey % this->_threadNum;
	log_debug("route task to thread %d (routerKey=%d,threadNum=%d)", index, routerKey, this->_threadNum);
	this->_threadVector[index]->pushAndNotify(task);
}/*}}}*/

WorkerTask* WorkerManager::decode(Buffer* buffer){/*{{{*/
	return this->_codec->decode(buffer);
}/*}}}*/

Buffer* WorkerManager::encode(WorkerOutcome* outcome){/*{{{*/
	return this->_codec->encode(outcome);
}/*}}}*/

int WorkerManager::threadNum(){/*{{{*/
	return this->_threadNum;
}/*}}}*/

size_t WorkerManager::taskQueueSize(size_t index){/*{{{*/
	if(0 <= index && index < this->_threadVector.size()){
		return this->_threadVector[index]->taskQueueSize();
	}

	return 0;
}/*}}}*/

size_t WorkerManager::outcomeQueueSize(size_t index){/*{{{*/
	if(0 <= index && index < this->_threadVector.size()){
		return this->_threadVector[index]->outcomeQueueSize();
	}

	return 0;
}/*}}}*/

void WorkerManager::addClientCallback(WorkerClientCallback* callback){/*{{{*/
	this->_clientCallbackList.push_back(callback);
}/*}}}*/

void WorkerManager::removeClientCallback(WorkerClientCallback* callback){/*{{{*/
	this->_clientCallbackList.remove(callback);
}/*}}}*/

void WorkerManager::onClientConnected(EventBase::Handler* handler){/*{{{*/
	list<WorkerClientCallback*>::iterator iter;
	for(iter = this->_clientCallbackList.begin(); iter != this->_clientCallbackList.end(); ++iter){
		(*iter)->onConnected(handler);	
	}
}/*}}}*/

void WorkerManager::onClientTask(EventBase::Handler* handler, WorkerTask* task){/*{{{*/
	list<WorkerClientCallback*>::iterator iter;
	for(iter = this->_clientCallbackList.begin(); iter != this->_clientCallbackList.end(); ++iter){
		(*iter)->onTask(handler, task);	
	}
}/*}}}*/

void WorkerManager::onClientClosed(EventBase::Handler* handler){/*{{{*/
	list<WorkerClientCallback*>::iterator iter;
	for(iter = this->_clientCallbackList.begin(); iter != this->_clientCallbackList.end(); ++iter){
		(*iter)->onClosed(handler);	
	}
}/*}}}*/

void WorkerManager::_addWorkerOutcomeHandler(WorkerThread* thread){/*{{{*/
	WorkerOutcomeHandler* handler = new WorkerOutcomeHandler();
	
	handler->fd = thread->outcomeReadFd;
	int ret = fcntl(handler->fd, F_SETFL, fcntl(handler->fd, F_GETFL) | O_NONBLOCK);
	if(ret == -1){
		log_error("set outcome read fd to nonblock fail, fd=%d", handler->fd);	
		exit(EXIT_FAILURE);
	}
	handler->connected = true;
	handler->maxIdleTime = 0;
	handler->thread = thread;
	EventBase::instance()->add(handler);
}/*}}}*/

void WorkerManager::_createThreadGroup(){/*{{{*/
	log_info("begin to create %d thread(s)", this->_threadNum);

	for(int i = 0; i < this->_threadNum; ++i){
		WorkerService* service = this->_config->createService();
		WorkerThread* thread = new WorkerThread(i, service, this->_taskQueueSizeThreshold, this->_outcomeQueueSizeThreshold);
		service->thread = thread;
		this->_addWorkerOutcomeHandler(thread);
		this->_threadVector.push_back(thread);
	}

	for(int i = 0; i < this->_threadNum; ++i){
		this->_threadVector[i]->create();	
	}
}/*}}}*/

