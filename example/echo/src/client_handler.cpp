#include "client_handler.h"

using namespace vcs;

void ClientHandler::onConnected(){
    log_debug("client connected, fd=%d", this->fd);
}

void ClientHandler::onClosed(){
    log_debug("client closed, fd=%d", this->fd);

    delete this;
}

void ClientHandler::onData(){
    this->output->append(this->input);
    this->input->shrink(this->input->size());
    EventBase::instance()->write(this);
}


