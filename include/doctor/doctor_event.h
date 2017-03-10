#ifndef _DOCTOR_EVENT_H
#define _DOCTOR_EVENT_H

#include "event_base/event_base.h"

class DoctorListener : public vcs::EventBase::Listener {
	public:
		void onAccepted(int);
		void onClosed();
};

class DoctorHandler : public vcs::EventBase::Handler {
	public:
		void onConnected();
		void onData();
		void onClosed();
};

#endif

