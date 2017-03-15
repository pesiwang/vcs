#ifndef CLIENT_LISTENER_H__
#define CLIENT_LISTENER_H__

#include "vcs/event_base/event_base.h"

class ClientListener : public vcs::EventBase::Listener
{
public:
    void onAccepted(int clientFd);
    void onClosed();
};

#endif

