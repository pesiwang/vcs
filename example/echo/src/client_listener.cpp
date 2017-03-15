#include "client_listener.h"
#include "client_handler.h"
#include "vcs/util/server_config.h"

using namespace vcs;

void ClientListener::onAccepted(int clientFd) {
    ClientHandler *handler = new ClientHandler();
    handler->fd = clientFd;
    handler->connected = true;
    handler->maxIdleTime = Singleton<ServerConfig>::instance()->getIntValue("network.client.max_idle_time");

    EventBase::instance()->add(handler);
    handler->onConnected();
}

void ClientListener::onClosed() {

}

