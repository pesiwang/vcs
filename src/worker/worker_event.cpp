#include <stdlib.h>
#include "worker/worker_event.h"
#include "worker/worker_manager.h"

using namespace vcs;

void WorkerListener::onAccepted(int clientFd){/*{{{*/
	WorkerClientHandler* handler = new WorkerClientHandler();
	handler->id = rand();
	handler->fd = clientFd;
	handler->connected = true;
	handler->maxIdleTime = this->clientMaxIdleTime; 

	EventBase::instance()->add(handler);
	handler->onConnected();
}/*}}}*/

void WorkerListener::onClosed(){/*{{{*/

}/*}}}*/


void WorkerClientHandler::onConnected(){/*{{{*/
	log_debug("worker client %d connected", this->fd);
	WorkerManager::instance()->onClientConnected(this);
}/*}}}*/

void WorkerClientHandler::onData(){/*{{{*/
	log_debug("worker client %d data available", this->fd);
	while(this->connected) {
		WorkerTask* task = WorkerManager::instance()->decode(this->input);
		if(NULL == task){
			// data not enough to decode
			log_debug("decode return null task, maybe not enough data to decode");
			return ;
		}

		if(task->corrupted){
			log_error("task corrupted");
			delete task;
			EventBase::instance()->remove(this);
			return ;
		}

		task->handlerId	= this->id; 
		task->clientFd	= this->fd;
		WorkerManager::instance()->routeTask(task);
		WorkerManager::instance()->onClientTask(this, task);
	}
}/*}}}*/

void WorkerClientHandler::onClosed(){/*{{{*/
	log_debug("worker client %d closed", this->fd);
	WorkerManager::instance()->onClientClosed(this);
	delete this;
}/*}}}*/


void WorkerOutcomeHandler::onConnected(){/*{{{*/

}/*}}}*/

void WorkerOutcomeHandler::encodeAndSendOutcome(){/*{{{*/
	WorkerOutcome* outcome = this->thread->popOutcome();
	if(NULL == outcome){
		log_error("recv outcome read notify, but no outcome in queue");
		return ;
	}

	Buffer* buffer = WorkerManager::instance()->encode(outcome);
	if(NULL == buffer){
		log_error("your codec->encode() return NULL, check your codec please");	
		delete outcome;
		return ;
	}

	Handler* handler = EventBase::instance()->getHandler(outcome->clientFd);
	if(NULL == handler || handler->id != outcome->handlerId){
		if(NULL == handler){
			log_warn("got outcome, but client handler fd not exist, clientFd=%d", outcome->clientFd);
		} else if(handler->id != outcome->handlerId){
			log_warn("got outcome, but client handler changed, clientFd=%d, outcome->handlerId=%d, now handlerId=%d", 
					outcome->clientFd, outcome->handlerId, handler->id);
		}

		delete outcome;
		delete buffer;
		return ;
	}

	handler->output->append(buffer);
	EventBase::instance()->write(handler);
	
	delete outcome;
	delete buffer;
}/*}}}*/

void WorkerOutcomeHandler::onData(){/*{{{*/
	char notifyOutcome[EACH_READ_NUM];

	while(this->connected){
		int len = (int)this->input->read(notifyOutcome, EACH_READ_NUM);
		if(len <= 0){
			break;
		}
		for(int i = 0; i < len; ++i) {
			switch(notifyOutcome[i]){
				case 'o':
					log_debug("recv outcome read notify, notify char=[o]");
					this->encodeAndSendOutcome();
					break;

				default:
					log_error("recv unknown outcome notify, notify char=[%c]", notifyOutcome[i]);
			}
		}
	}
}/*}}}*/

void WorkerOutcomeHandler::onClosed(){/*{{{*/
	delete this;
}/*}}}*/


void WorkerObserver::onStarted(){/*{{{*/
	log_info("WorkerObserver::onStarted");
}/*}}}*/

void WorkerObserver::onStopping(){/*{{{*/
	log_info("WorkerObserver::onStopping");
	WorkerManager::instance()->stop();
}/*}}}*/

void WorkerObserver::onStopped(){/*{{{*/
	log_info("WorkerObserver::onStopped");
}/*}}}*/

void WorkerObserver::onTimer(){/*{{{*/
}/*}}}*/

