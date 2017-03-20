#ifndef _GROUP_CODEC_H
#define _GROUP_CODEC_H

#include "vcs/worker/worker_manager.h"
#include <string>

#define GROUP_VERSION				0x01

#define CMD_CREATE_GROUP			0x01
#define CMD_RESP_CREATE_GROUP		0x81

#define CMD_DISMISS_GROUP			0x02
#define CMD_RESP_DISMISS_GROUP		0x82

#define CMD_JOIN_GROUP				0x03
#define CMD_RESP_JOIN_GROUP			0x83

#define CMD_QUIT_GROUP				0x04
#define CMD_RESP_QUIT_GROUP			0x84

#define CMD_MSG_GROUP				0x05
#define CMD_RESP_MSG_GROUP			0x85

#define CMD_REGISTER_CHAT			0x06
#define CMD_RESP_REGISTER_CHAT		0x86

#define CMD_GET_GROUP_INFO			0x07
#define CMD_RESP_GET_GROUP_INFO		0x87

#define CMD_SET_GROUP_STATUS		0x08
#define CMD_RESP_SET_GROUP_STATUS	0x88

#define CMD_BROADCAST_GROUP			0x09
#define CMD_RESP_BROADCAST_GROUP	0x89

class GroupCodec : public vcs::WorkerCodec {
	public:
		vcs::WorkerTask* decode(vcs::Buffer* buffer);
		vcs::Buffer* encode(vcs::WorkerOutcome* outcome);

	private:
		vcs::WorkerTask* _createTask(unsigned char cmd);
		void _parseBody(vcs::WorkerTask* task, const std::string& body);
};

#endif
