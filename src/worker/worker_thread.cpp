#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "event_base/event_base.h"
#include "worker/worker_thread.h"
#include "worker/worker_manager.h"
#include "worker/worker_event.h"

using namespace vcs;

WorkerThread::WorkerThread(int threadIndex, WorkerService* service, 
		unsigned int taskQueueSizeThreshold, 
		unsigned int outcomeQueueSizeThreshold) : status(THREAD_STATUS_STOPPED) 
{/*{{{*/

	this->threadIndex	= threadIndex;
	this->service		= service;
	this->_taskQueueSizeThreshold 		= taskQueueSizeThreshold;
	this->_outcomeQueueSizeThreshold	= outcomeQueueSizeThreshold;
	this->_init();
}/*}}}*/

WorkerThread::~WorkerThread(){/*{{{*/
	delete this->service;
	pthread_mutex_destroy(&(this->_taskQueueMutex));
	pthread_mutex_destroy(&(this->_outcomeQueueMutex));
	::close(this->taskReadFd);
	::close(this->taskWriteFd);
	::close(this->outcomeReadFd);
	::close(this->outcomeWriteFd);
}/*}}}*/

void WorkerThread::create(){/*{{{*/
	this->status = THREAD_STATUS_RUNNING;
	int ret = pthread_create(&(this->threadId), NULL, WorkerThread::threadEntry, this);
	if(ret != 0){
		log_error("create thread fail, errno=%d, error=%s", ret, strerror(errno));
		exit(EXIT_FAILURE);
	}
}/*}}}*/

void WorkerThread::_check(){/*{{{*/
	pthread_mutex_lock(&(this->_taskQueueMutex));
	size_t taskQueueSize = this->_taskQueue.size();
	pthread_mutex_unlock(&(this->_taskQueueMutex));
	
	if(taskQueueSize > this->_taskQueueSizeThreshold){
		log_error("threadVector[%d] task queue size now:%d", this->threadIndex, (int)taskQueueSize);
	}

	pthread_mutex_lock(&(this->_outcomeQueueMutex));
	size_t outcomeQueueSize = this->_outcomeQueue.size();
	pthread_mutex_unlock(&(this->_outcomeQueueMutex));

	if(outcomeQueueSize > this->_outcomeQueueSizeThreshold){
		log_error("threadVector[%d] outcome queue size now:%d", this->threadIndex, (int)outcomeQueueSize);
	}
}/*}}}*/

void WorkerThread::stop(){/*{{{*/
	char notifyQuit[1] = {'q'};
	int ret = ::write(this->taskWriteFd, notifyQuit, 1);
	if(ret != 1){
	    log_error("notify stop fail");
	}
}/*}}}*/

void* WorkerThread::threadEntry(void* arg){/*{{{*/

	WorkerThread* thread = (WorkerThread*)arg;
	char notifyTask[EACH_READ_NUM];

	while(THREAD_STATUS_RUNNING == thread->status){
		int nread = ::read(thread->taskReadFd, notifyTask, EACH_READ_NUM);
		if(nread == -1){
			log_error("read from taskReadFd fail, errno=%d, error=%s", errno, strerror(errno));
			continue;
		}
		for(int i = 0; i < nread; ++i){
			switch(notifyTask[i]){
				case 't':
					log_debug("recv task read notify, notify char=[t]");
					thread->_doWork();
					break;

				case 'q':
					log_info("recv stop thread notify, notify char=[q]");
					thread->status = THREAD_STATUS_STOPPING;
					break;

				default:
					log_error("recv unknown task notify, notify char=[%c]", notifyTask[i]);
			}
		}
		if(nread >= EACH_READ_NUM){
			thread->_check();
		}
	}

	thread->status = THREAD_STATUS_STOPPED;

	log_info("threadVector[%d] stopped, lost %d task, %d outcome", thread->threadIndex, 
				(int)thread->_taskQueue.size(), (int)thread->_outcomeQueue.size());

	return NULL;
}/*}}}*/

void WorkerThread::pushAndNotify(WorkerTask* task){/*{{{*/
	pthread_mutex_lock(&(this->_taskQueueMutex));
	this->_taskQueue.push(task);
	pthread_mutex_unlock(&(this->_taskQueueMutex));

	char notifyTask[1] = {'t'};
	int ret = ::write(this->taskWriteFd, notifyTask, 1);
	if(ret != 1){
		log_error("write to taskWriteFd fail");
	}
}/*}}}*/

void WorkerThread::pushAndNotify(WorkerOutcome* outcome){/*{{{*/
	pthread_mutex_lock(&(this->_outcomeQueueMutex));
	this->_outcomeQueue.push(outcome);
	pthread_mutex_unlock(&(this->_outcomeQueueMutex));

	char notifyOutcome[1] = {'o'};
	int ret = ::write(this->outcomeWriteFd, notifyOutcome, 1);
	if(ret != 1){
		log_error("write to outcomeWriteFd fail");
	}
}/*}}}*/

WorkerTask* WorkerThread::popTask(){/*{{{*/
	pthread_mutex_lock(&(this->_taskQueueMutex));
	if(this->_taskQueue.size() <= 0){
		pthread_mutex_unlock(&(this->_taskQueueMutex));
		return NULL;
	}
	WorkerTask* task = this->_taskQueue.front();
	this->_taskQueue.pop();
	pthread_mutex_unlock(&(this->_taskQueueMutex));
	
	return task;
}/*}}}*/

WorkerOutcome* WorkerThread::popOutcome(){/*{{{*/
	pthread_mutex_lock(&(this->_outcomeQueueMutex));
	if(this->_outcomeQueue.size() <= 0){
		return NULL;
	}
	WorkerOutcome* outcome = this->_outcomeQueue.front();
	this->_outcomeQueue.pop();
	pthread_mutex_unlock(&(this->_outcomeQueueMutex));
	
	return outcome;

}/*}}}*/

size_t WorkerThread::taskQueueSize(){/*{{{*/
	pthread_mutex_lock(&(this->_taskQueueMutex));
	size_t size = this->_taskQueue.size();
	pthread_mutex_unlock(&(this->_taskQueueMutex));

	return size;
}/*}}}*/

size_t WorkerThread::outcomeQueueSize(){/*{{{*/
	pthread_mutex_lock(&(this->_outcomeQueueMutex));
	size_t size = this->_outcomeQueue.size();
	pthread_mutex_unlock(&(this->_outcomeQueueMutex));

	return size;
}/*}}}*/

void WorkerThread::_doWork(){/*{{{*/
	WorkerTask* task = this->popTask();
	if(task == NULL){
		log_error("recv task read notify, but no task in queue");	
		return ;
	}

	WorkerOutcome* outcome = this->service->run(task);
	if(outcome != NULL){
		outcome->handlerId	= task->handlerId;
		outcome->clientFd	= task->clientFd;
		this->pushAndNotify(outcome);
	}
	delete task;
}/*}}}*/

void WorkerThread::_init(){/*{{{*/
	int taskFds[2];
	if(pipe(taskFds) == -1){
		log_error("create task pipe fds fail");
		exit(EXIT_FAILURE);
	}	
	this->taskReadFd	= taskFds[0];
	this->taskWriteFd	= taskFds[1];
	log_info("threadVector[%d]: taskReadFd=%d, taskWriteFd=%d", this->threadIndex, this->taskReadFd, this->taskWriteFd);

	int outcomeFds[2];
	if(pipe(outcomeFds) == -1){
		log_error("create outcome pipe fds fail");
		exit(EXIT_FAILURE);
	}
	this->outcomeReadFd	= outcomeFds[0];
	this->outcomeWriteFd	= outcomeFds[1];
	log_info("threadVector[%d]: outcomeReadFd=%d, outcomeWriteFd=%d", this->threadIndex, this->outcomeReadFd, this->outcomeWriteFd);

	pthread_mutex_init(&(this->_taskQueueMutex), NULL);
	pthread_mutex_init(&(this->_outcomeQueueMutex), NULL);
}/*}}}*/
