#include "group_callback.h"
#include "chat_manager.h"

using namespace vcs;

void ChatClientCallback::onConnected(EventBase::Handler* handler){

}

void ChatClientCallback::onTask(EventBase::Handler* handler, WorkerTask* task){

}

void ChatClientCallback::onClosed(EventBase::Handler* handler){
	if(ChatManager::instance()->isChatFd(handler->fd)){
		ChatManager::instance()->unregisterChat(handler->fd);
		log_info("unregister chat fd %d, chat handler id %d", handler->fd, handler->id);
	}
}

