
#ifndef _MODEL_GROUP_STORAGE_H
#define _MODEL_GROUP_STORAGE_H

#include "hiredis/hiredis.h"
#include <string>


class GroupStorage{
	public:
		GroupStorage();
		virtual ~GroupStorage();

		bool connect();
		bool isConnected();
		bool get(const std::string& hashKey, const std::string& groupId, std::string& content);
		bool set(const std::string& hashKey, const std::string& groupId, const std::string& content);
		bool del(const std::string& haskKey, const std::string& groupId);

	public:
		std::string host;
		int port;

    private:
        bool auth();
	private:
		redisContext*	_context;
};

#endif
