#ifndef _CLIENT_SYNC_CLIENT_H
#define _CLIENT_SYNC_CLIENT_H

#include "event_base/event_base.h"
#include <string>

class SyncClient {
	public:
		SyncClient(const std::string& host, unsigned short port);
		virtual ~SyncClient();
		bool send(vcs::Buffer*);
		bool recv(vcs::Buffer*);

	private:
		void _connect();
		void _close();

		int	_fd;
		std::string		_host;
		unsigned short	_port;

};


#endif

