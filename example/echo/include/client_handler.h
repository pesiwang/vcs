#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H


#include "vcs/event_base/event_base.h"

class ClientHandler : public vcs::EventBase::Handler {
public:
    void onConnected();
    void onData();
    void onClosed();
};

#endif

