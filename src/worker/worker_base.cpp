#include "event_base/event_base.h"
#include "worker/worker_base.h"
#include "worker/worker_event.h"

using namespace vcs;

/** ========= WorkerService ============= **/
void WorkerService::write(int clientFd, int handlerId, WorkerOutcome* outcome){
	outcome->clientFd   = clientFd;
	outcome->handlerId  = handlerId;
	this->thread->pushAndNotify(outcome);
}


