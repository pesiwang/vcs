#ifndef _CHAT_MANAGER_H
#define _CHAT_MANAGER_H

#include <pthread.h>
#include <set>
#include <vector>

class ChatManager {
	public:
		class Entity{
			public:
				int chatFd;
				int chatHandlerId;
		};

	public:
		static ChatManager* instance();
		bool isChatFd(int clientFd);
		Entity getChat(unsigned int key);
		void registerChat(int chatFd, int chatHandlerId);
		void unregisterChat(int clientFd);

	private:
		ChatManager(){
			pthread_mutex_init(&(this->_chatMutex), NULL);
		};
		virtual ~ChatManager(){
			pthread_mutex_destroy(&(this->_chatMutex));
		};
		
		pthread_mutex_t _chatMutex;

		std::set<int> _chatFdSet;
		std::vector<Entity> _entityVector;
		
};

#endif
