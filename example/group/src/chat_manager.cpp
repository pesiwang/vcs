#include "chat_manager.h"
#include "vcs/event_base/event_base.h"

using namespace std;

ChatManager* ChatManager::instance(){
	static ChatManager self;

	return &self;
}

bool ChatManager::isChatFd(int clientFd){
	pthread_mutex_lock(&(this->_chatMutex));

	bool isChatFd = false;
	set<int>::iterator iter = this->_chatFdSet.find(clientFd);
	if(iter != this->_chatFdSet.end()){
		isChatFd = true;
	}

	pthread_mutex_unlock(&(this->_chatMutex));

	return isChatFd;
}

ChatManager::Entity ChatManager::getChat(unsigned int key){
	ChatManager::Entity entity;
	entity.chatFd			= -1;
	entity.chatHandlerId	= -1;

	int index = 0;
	pthread_mutex_lock(&(this->_chatMutex));
	
	if(this->_entityVector.size() > 0){
		index = key % this->_entityVector.size();
		entity = this->_entityVector[index];
	}

	pthread_mutex_unlock(&(this->_chatMutex));

	return entity;
}

void ChatManager::registerChat(int chatFd, int chatHandlerId){
	ChatManager::Entity entity;
	entity.chatFd			= chatFd;
	entity.chatHandlerId	= chatHandlerId;

	pthread_mutex_lock(&(this->_chatMutex));

	pair<set<int>::iterator, bool> result = this->_chatFdSet.insert(chatFd);
	if(result.second){
		this->_entityVector.push_back(entity);
	} else {
		for(vector<ChatManager::Entity>::iterator iter = this->_entityVector.begin(); iter != this->_entityVector.end(); ++iter){
			if(iter->chatFd == chatFd){
				iter->chatHandlerId = chatHandlerId;
				break;
			}
		}
	}

	size_t chatNum = this->_entityVector.size();
	pthread_mutex_unlock(&(this->_chatMutex));
	log_info("chat_manager: registerChat chatFd=%d, chatHandlerId=%d, now has %d chat server connected", chatFd, chatHandlerId, (int)chatNum);
}

void ChatManager::unregisterChat(int chatFd){
	pthread_mutex_lock(&(this->_chatMutex));

	if(this->_chatFdSet.find(chatFd) != this->_chatFdSet.end()){
		this->_chatFdSet.erase(chatFd);
		for(vector<ChatManager::Entity>::iterator iter = this->_entityVector.begin(); iter != this->_entityVector.end(); ++iter){
			if(iter->chatFd == chatFd){
				this->_entityVector.erase(iter);
				break;
			}
		}
	}

	size_t chatNum = this->_entityVector.size();   
	pthread_mutex_unlock(&(this->_chatMutex));
	log_info("chat_manager: unregisterChat chatFd=%d, now has %d chat server connected", chatFd, (int)chatNum);
}

