#include "doctor/ability_status.h"
#include <sstream>
#include "event_base/event_base.h"
#include "worker/worker_manager.h"

using namespace std;
using namespace vcs;

string AbilityStatus::process(vector<string> paramVector){/*{{{*/
	if(paramVector.size() <= 0){
		return this->_all();
	}

	string statusCmd	= paramVector[0];
	log_debug("status cmd is %s", statusCmd.c_str());
	if(statusCmd.compare("server") == 0){
		return this->_server();
	} else if(statusCmd.compare("worker") == 0){
		return this->_worker();
	} else {
		return "error: unknown status section\n";
	}
}/*}}}*/

string AbilityStatus::help(){
	stringstream ss;

	ss << "status usage:\n"
		<< "status\n"
		<< "status server\n"
		<< "status worker\n";

	return ss.str();
}

void AbilityStatus::onTimer(){/*{{{*/
	// do nothing
}/*}}}*/

string AbilityStatus::_all(){
	stringstream ss;

	ss << "--------server--------\n";
	ss << this->_server() << "\n";
	ss << "--------worker--------\n";
	ss << this->_worker() << "\n";

	return ss.str();
}

string AbilityStatus::_server(){
	stringstream ss;

	ss << "listener\t: " << EventBase::instance()->listenerNum() << "\n"
		<< "observer\t: " << EventBase::instance()->observerNum() << "\n"
		<< "handler\t\t: " << EventBase::instance()->handlerNum() << "\n";

	return ss.str();
}

string AbilityStatus::_worker(){
	stringstream ss;

	int threadNum = WorkerManager::instance()->threadNum();
	ss << "worker thread\t: " << threadNum << "\n";
	for(int i = 0; i < threadNum; ++i){
		ss<< "threadVector[" << i << "] task queue\t: " << WorkerManager::instance()->taskQueueSize(i) << "\n"
			<< "threadVector[" << i << "] outcome queue\t: " << WorkerManager::instance()->outcomeQueueSize(i) << "\n";
	}

	return ss.str();
}


