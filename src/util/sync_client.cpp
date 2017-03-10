#include "util/sync_client.h"
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

using namespace std;
using namespace vcs;

SyncClient::SyncClient(const string& host, unsigned short port) : _fd(-1){
	this->_host	= host;
	this->_port	= port;
	this->_connect();
}

SyncClient::~SyncClient(){
	this->_close();
}

bool SyncClient::send(Buffer* buffer){/*{{{*/
	if(this->_fd == -1){
		return false;
	}

	char buf[1024];
	int curr_read = 0;
	while((curr_read = buffer->peek(buf, 1024)) > 0){
		int n_written = 0;
		int n_left = curr_read;
		while(n_left > 0){
			int written = ::write(this->_fd, buf + n_written, n_left);
			if(written == -1 && errno == EINTR){
				continue ;
			}
			if(written == -1){
				return false;
			}
			n_written += written;
			n_left -= written;
		}

		buffer->shrink(n_written);
	}

	return true;
}/*}}}*/

bool SyncClient::recv(Buffer* buffer){/*{{{*/
	if(this->_fd == -1){
		return false;
	}

	char buf[1024];
	int curr_read = ::read(this->_fd, buf, 1024);
	if(curr_read > 0){
		buffer->write(buf, curr_read);
	} else if(curr_read == 0){
		return true;
	} else {
		if(errno == EINTR){
			return true;
		}
		return false;
	}

	return true;
}/*}}}*/

void SyncClient::_connect(){/*{{{*/
	this->_fd = ::socket(AF_INET, SOCK_STREAM, 0);
	if(this->_fd == -1){
		return ;
	}

	// set connect timeout (connect timeout use SO_SNDTIMO value)
	struct timeval connectTimeout = {2, 0}; // 2 seconds
	if(setsockopt(this->_fd, SOL_SOCKET, SO_SNDTIMEO, &connectTimeout, sizeof(connectTimeout)) == -1){
		log_error("setsockopt set connect timeout fail");
	}

	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_addr.s_addr = inet_addr(this->_host.c_str());
	sin.sin_family = AF_INET;
	sin.sin_port = htons(this->_port);

	int res = ::connect(this->_fd, (struct sockaddr *)&sin, sizeof(sin));
	if (res == -1){
		this->_close();
		return ;
	}

	//set send and recv timeout
	struct timeval dataTimeout = {3, 0};
	if(setsockopt(this->_fd, SOL_SOCKET, SO_SNDTIMEO, &dataTimeout, sizeof(dataTimeout)) == -1){
		log_error("setsockopt set send timeout fail");
	}

	if(setsockopt(this->_fd, SOL_SOCKET, SO_RCVTIMEO, &dataTimeout, sizeof(dataTimeout)) == -1){
		log_error("setsockopt set recv timeout fail");
	}
}/*}}}*/

void SyncClient::_close(){/*{{{*/
	if(this->_fd != -1){
		::close(this->_fd);
		this->_fd = -1;
	}
}/*}}}*/

