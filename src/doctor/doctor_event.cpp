#include "doctor/doctor_event.h"
#include "doctor/doctor_codec.h"
#include "doctor/server_doctor.h"
#include "event_base/event_base.h"

using namespace std;
using namespace vcs;

void DoctorListener::onAccepted(int clientFd){/*{{{*/
	DoctorHandler* handler = new DoctorHandler();
	handler->fd			= clientFd;
	handler->connected	= true;
	handler->maxIdleTime	= 600; 

	EventBase::instance()->add(handler);
	handler->onConnected();
}/*}}}*/

void DoctorListener::onClosed(){/*{{{*/
	
}/*}}}*/


void DoctorHandler::onConnected(){/*{{{*/
	log_debug("doctor client %d connected", this->fd);
}/*}}}*/

void DoctorHandler::onData(){/*{{{*/
	while(this->connected){
		DoctorCodec::Request* request = DoctorCodec::decode(this->input);
		if(request == NULL){
			return ;
		}
		
		string response = ServerDoctor::instance()->process(request->abilityName, request->paramVector);
		Buffer* buffer = DoctorCodec::encode(response);
		Handler* handler = EventBase::instance()->getHandler(this->fd);
		if(handler == NULL){
			log_error("doctor client %d handler not existed", this->fd);
			delete request;

			return ;
		}

		handler->output->append(buffer);
		EventBase::instance()->write(handler);

		delete request;
		delete buffer;
	}
}/*}}}*/

void DoctorHandler::onClosed(){/*{{{*/
	log_debug("doctor client %d closed", this->fd);
	delete this;
}/*}}}*/


