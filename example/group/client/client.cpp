#include <iostream>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/resource.h>
#include "protobuf/group_package.pb.h"
#include "vcs/event_base/event_base.h"
#include "vcs/util/vc_codec.h"
#include "group_codec.h"

#define SERVER_HOST		"127.0.0.1"
#define SERVER_PORT		6666

using namespace std;
using namespace vcs;
using namespace group;

int g_fd;

void printHelp(){/*{{{*/
	cout << "------------input cmd---------------------" << endl;
	cout << "help: print this" << endl;
	cout << "create: create group" << endl;
	cout << "get: get group info" << endl;
	cout << "dismiss: dismiss group" << endl;
	cout << "join: join group" << endl;
	cout << "quit: quit group" << endl;
	cout << "msg: msg group" << endl;
	cout << "register_chat: register as chat server" << endl;
	cout << "recv_msg: recv group msg response(chat server only)" << endl;
	cout << "set_group_status: set group status(0:normal, 1:gray, 2:black)" << endl;
	cout << "broadcast: broadcast group(super user only)" << endl;
	cout << "-------------------------------------------" << endl;
	cout << endl;
}/*}}}*/

void readResp(Buffer* buffer){/*{{{*/
	VCCodec::PACKAGE_HEAD head;
	cout << "wait resp..." << endl;
	char buf[1024];
	while(true){
		
		int curr_read = 0;
		if((curr_read = read(g_fd, buf, 1024)) > 0){
			buffer->write(buf, curr_read);
			cout << "recved " << buffer->size() << " bytes" << endl;
		}

		if(buffer->size() < sizeof(head)){
			continue;
		}

		buffer->peek((char*)&head, sizeof(head));
		head.length = ntohs(head.length);
		if(buffer->size() < head.length){
			continue;
		}
		break;

	}
	cout << "recv done" << endl;	
}/*}}}*/

void createGroup(){/*{{{*/
	string userId;
	string groupId;

	cout << "input userId:";
	cin >> userId;

	cout << "groupId:"; cin >> groupId;

	cout << "create group with userId=" << userId << ",groupId=" << groupId << endl;

	Buffer  buffer;
	RequestCreateGroup req;
	req.set_group_id(groupId);
	req.set_user_id(userId);
	string str;
	req.SerializeToString(&str);
	
	VCCodec::PACKAGE_HEAD head;
	head.version	= GROUP_VERSION;
	head.length		= sizeof(head) + str.size();
	head.length		= htons(head.length);
	head.cmd		= CMD_CREATE_GROUP;
	buffer.write((char*)&head, sizeof(head));
	buffer.write(str.data(), str.size());
	if(!Helper::Socket::write(g_fd, &buffer)){
		cout << "send fail" << endl;
	} else {
		cout << "send succ, begin to read" << endl;
	}

	Buffer inBuffer;
	readResp(&inBuffer);
	inBuffer.read((char*)&head, sizeof(head));

	string data(head.length, 0);
	inBuffer.read(data.data(), head.length);
	ResponseCreateGroup resp;
	resp.ParseFromString(data);
	cout << "create result:" << resp.result() << endl;
}
/*}}}*/

void getGroupInfo(){/*{{{*/
	string userId;
	string groupId;
	string content;


	cout << "groupId:"; cin >> groupId;
	cout << "get group info with groupId=" << groupId << endl;

	Buffer  buffer;
	RequestGetGroupInfo req;
	req.set_group_id(groupId);
	string str;
	req.SerializeToString(&str);
	
	VCCodec::PACKAGE_HEAD head;
	head.version	= GROUP_VERSION;
	head.length		= sizeof(head) + str.size();
	head.length		= htons(head.length);
	head.cmd		= CMD_GET_GROUP_INFO;
	buffer.write((char*)&head, sizeof(head));
	buffer.write(str.data(), str.size());
	if(!Helper::Socket::write(g_fd, &buffer)){
		cout << "send fail" << endl;
	} else {
		cout << "send succ, begin to read" << endl;
	}

	Buffer inBuffer;
	readResp(&inBuffer);
	inBuffer.read((char*)&head, sizeof(head));

	string data(head.length, 0);
	inBuffer.read(data.data(), head.length);
	ResponseGetGroupInfo resp;
	resp.ParseFromString(data);
	cout << "get group info result:" << resp.result() << endl;
	cout << "groupId:" << resp.group_id() << endl;
	cout << "creator:" << resp.creator() << endl;
	cout << "status:" << resp.status() << endl;
	cout << "member num :" << resp.member_size() << endl;
	for(int i = 0; i < resp.member_size(); ++i){
		GroupMember member = resp.member(i);
		cout << "member["<<i<<"] id:"<< member.id() << ", activeTime:" << member.active_time() 
			<< ",joinTime:" << member.join_time() << endl;
	} 
	cout << endl;
}
/*}}}*/

void getGroupInfoRepated(){/*{{{*/
	string userId = "pesi";
	string groupId = "pesi_group";

	Buffer  buffer;
	RequestGetGroupInfo req;
	req.set_group_id(groupId);
	string str;
	req.SerializeToString(&str);
	
	VCCodec::PACKAGE_HEAD head;
	head.version	= GROUP_VERSION;
	head.length		= sizeof(head) + str.size();
	head.length		= htons(head.length);
	head.cmd		= CMD_GET_GROUP_INFO;
	buffer.write((char*)&head, sizeof(head));
	buffer.write(str.data(), str.size());
	if(!Helper::Socket::write(g_fd, &buffer)){
		cout << "send fail" << endl;
	} else {
		cout << "send succ, begin to read" << endl;
	}

	Buffer inBuffer;
	readResp(&inBuffer);
	inBuffer.read((char*)&head, sizeof(head));

	string data(head.length, 0);
	inBuffer.read(data.data(), head.length);
	ResponseGetGroupInfo resp;
	resp.ParseFromString(data);
	cout << "get group info result:" << resp.result() << endl;
	cout << "groupId:" << resp.group_id() << endl;
	cout << "creator:" << resp.creator() << endl;
	cout << "member num :" << resp.member_size() << endl;
	for(int i = 0; i < resp.member_size(); ++i){
		GroupMember member = resp.member(i);
		cout << "member["<<i<<"] id:"<< member.id() << ", activeTime:" << member.active_time() 
			<< ",joinTime:" << member.join_time() << endl;
	} 
	cout << endl;
}
/*}}}*/

void dismissGroup(){/*{{{*/
	string userId;
	string groupId;

	cout << "input userId:";
	cin >> userId;

	cout << "groupId:"; cin >> groupId;

	cout << "dismiss group with userId=" << userId << ",groupId=" << groupId << endl;

	Buffer  buffer;
	RequestDismissGroup req;
	req.set_group_id(groupId);
	req.set_user_id(userId);
	string str;
	req.SerializeToString(&str);
	
	VCCodec::PACKAGE_HEAD head;
	head.version	= GROUP_VERSION;
	head.length		= sizeof(head) + str.size();
	head.length		= htons(head.length);
	head.cmd		= CMD_DISMISS_GROUP;
	buffer.write((char*)&head, sizeof(head));
	buffer.write(str.data(), str.size());
	if(!Helper::Socket::write(g_fd, &buffer)){
		cout << "send fail" << endl;
	} else {
		cout << "send succ, begin to read" << endl;
	}

	Buffer inBuffer;
	readResp(&inBuffer);
	inBuffer.read((char*)&head, sizeof(head));

	string data(head.length, 0);
	inBuffer.read(data.data(), head.length);
	ResponseDismissGroup resp;
	resp.ParseFromString(data);
	cout << "dismiss result:" << resp.result() << endl;
}
/*}}}*/

void joinGroup(){/*{{{*/
	string userId;
	string groupId;

	cout << "input userId:";
	cin >> userId;

	cout << "groupId:"; cin >> groupId;

	cout << "join group with userId=" << userId << ",groupId=" << groupId << endl;

	Buffer  buffer;
	RequestJoinGroup req;
	req.set_group_id(groupId);
	req.set_user_id(userId);
	string str;
	req.SerializeToString(&str);
	
	VCCodec::PACKAGE_HEAD head;
	head.version	= GROUP_VERSION;
	head.length		= sizeof(head) + str.size();
	head.length		= htons(head.length);
	head.cmd		= CMD_JOIN_GROUP;
	buffer.write((char*)&head, sizeof(head));
	buffer.write(str.data(), str.size());
	if(!Helper::Socket::write(g_fd, &buffer)){
		cout << "send fail" << endl;
	} else {
		cout << "send succ, begin to read" << endl;
	}

	Buffer inBuffer;
	readResp(&inBuffer);
	inBuffer.read((char*)&head, sizeof(head));

	string data(head.length, 0);
	inBuffer.read(data.data(), head.length);
	ResponseJoinGroup resp;
	resp.ParseFromString(data);
	cout << "join result:" << resp.result() << endl;
}
/*}}}*/

void quitGroup(){/*{{{*/
	string userId;
	string groupId;

	cout << "input userId:";
	cin >> userId;

	cout << "groupId:"; cin >> groupId;

	cout << "quit group with userId=" << userId << ",groupId=" << groupId << endl;

	Buffer  buffer;
	RequestQuitGroup req;
	req.set_group_id(groupId);
	req.set_user_id(userId);
	string str;
	req.SerializeToString(&str);
	
	VCCodec::PACKAGE_HEAD head;
	head.version	= GROUP_VERSION;
	head.length		= sizeof(head) + str.size();
	head.length		= htons(head.length);
	head.cmd		= CMD_QUIT_GROUP;
	buffer.write((char*)&head, sizeof(head));
	buffer.write(str.data(), str.size());
	if(!Helper::Socket::write(g_fd, &buffer)){
		cout << "send fail" << endl;
	} else {
		cout << "send succ, begin to read" << endl;
	}

	Buffer inBuffer;
	readResp(&inBuffer);
	inBuffer.read((char*)&head, sizeof(head));

	string data(head.length, 0);
	inBuffer.read(data.data(), head.length);
	ResponseQuitGroup resp;
	resp.ParseFromString(data);
	cout << "quit result:" << resp.result() << endl;
}
/*}}}*/

void registerChat(){/*{{{*/

	cout << "register chat server" << endl;

	Buffer  buffer;
	
	VCCodec::PACKAGE_HEAD head;
	head.version	= GROUP_VERSION;
	head.length		= sizeof(head);
	head.length		= htons(head.length);
	head.cmd		= CMD_REGISTER_CHAT;
	buffer.write((char*)&head, sizeof(head));
	if(!Helper::Socket::write(g_fd, &buffer)){
		cout << "send fail" << endl;
	} else {
		cout << "send succ, begin to read" << endl;
	}

	Buffer inBuffer;
	readResp(&inBuffer);
	inBuffer.read((char*)&head, sizeof(head));

	string data(head.length, 0);
	inBuffer.read(data.data(), head.length);
	ResponseRegisterChat resp;
	resp.ParseFromString(data);
	cout << "register chat server result:" << resp.result() << endl;
}
/*}}}*/

void msgGroup(){/*{{{*/
	static int mid = 0;
	string userId;
	string groupId;
	string content;

	cout << "input userId:";cin >> userId;

	cout << "groupId:"; cin >> groupId;
	cout << "content:"; cin >> content;
	cout << "msg group with userId=" << userId << ",groupId=" << groupId << ",content=" << content << endl;

	Buffer  buffer;
	RequestMsgGroup req;
	req.set_group_id(groupId);
	req.set_sender_id(userId);
	mid++;
	char mid_str[10];
	sprintf(mid_str, "mid:%d", mid);
	req.set_mid(string(mid_str));
	req.set_type(0);
	req.set_content(content);
	req.set_time(time(NULL));
	string str;
	req.SerializeToString(&str);
	
	VCCodec::PACKAGE_HEAD head;
	head.version	= GROUP_VERSION;
	head.length		= sizeof(head) + str.size();
	head.length		= htons(head.length);
	head.cmd		= CMD_MSG_GROUP;
	buffer.write((char*)&head, sizeof(head));
	buffer.write(str.data(), str.size());
	if(!Helper::Socket::write(g_fd, &buffer)){
		cout << "send fail" << endl;
	} else {
		cout << "send succ, begin to read" << endl;
	}

	Buffer inBuffer;
	readResp(&inBuffer);
	inBuffer.read((char*)&head, sizeof(head));

	string data(head.length, 0);
	inBuffer.read(data.data(), head.length);
	ResponseMsgGroup resp;
	resp.ParseFromString(data);
	cout << "msg result:" << resp.result() << endl;
	cout << "recver:";
	for(int i = 0; i < resp.recver_size(); ++i){
		cout << resp.recver(i) << ",";
	}
	cout << endl;
	cout << "content:" << resp.request().content()<<endl;
}
/*}}}*/

void broadcastGroup(){/*{{{*/
	static int mid = 0;
	string userId;
	string groupId;
	string content;

	cout << "input userId:";cin >> userId;

	cout << "groupId:"; cin >> groupId;
	cout << "content:"; cin >> content;
	cout << "msg group with userId=" << userId << ",groupId=" << groupId << ",content=" << content << endl;

	Buffer  buffer;
	RequestMsgGroup req;
	req.set_group_id(groupId);
	req.set_sender_id(userId);
	mid++;
	char mid_str[10];
	sprintf(mid_str, "mid:%d", mid);
	req.set_mid(string(mid_str));
	req.set_type(0);
	req.set_content(content);
	req.set_time(time(NULL));
	string str;
	req.SerializeToString(&str);
	
	VCCodec::PACKAGE_HEAD head;
	head.version	= GROUP_VERSION;
	head.length		= sizeof(head) + str.size();
	head.length		= htons(head.length);
	head.cmd		= CMD_BROADCAST_GROUP;
	buffer.write((char*)&head, sizeof(head));
	buffer.write(str.data(), str.size());
	if(!Helper::Socket::write(g_fd, &buffer)){
		cout << "send fail" << endl;
	} else {
		cout << "send succ, begin to read" << endl;
	}

	Buffer inBuffer;
	readResp(&inBuffer);
	inBuffer.read((char*)&head, sizeof(head));

	string data(head.length, 0);
	inBuffer.read(data.data(), head.length);
	ResponseBroadcastGroup resp;
	resp.ParseFromString(data);
	cout << "msg result:" << resp.result() << endl;
	cout << "recver:";
	for(int i = 0; i < resp.recver_size(); ++i){
		cout << resp.recver(i) << ",";
	}
	cout << endl;
	cout << "content:" << resp.request().content()<<endl;
}
/*}}}*/

void recvMsg(){/*{{{*/
	VCCodec::PACKAGE_HEAD head;

	Buffer inBuffer;
	readResp(&inBuffer);
	inBuffer.read((char*)&head, sizeof(head));

	string data(head.length, 0);
	inBuffer.read(data.data(), head.length);
	ResponseMsgGroup resp;
	resp.ParseFromString(data);
	cout << "msg result:" << resp.result() << endl;
	cout << "recver:";
	for(int i = 0; i < resp.recver_size(); ++i){
		cout << resp.recver(i) << ",";
	}
	cout << endl;
	cout << "content:" << resp.request().content()<<endl;
}/*}}}*/

void setGroupStatus(){/*{{{*/
	int status;
	string groupId;

	cout << "input status:";cin >> status;
	cout << "groupId:"; cin >> groupId;
	cout << "set group status with status=" << status << ",groupId=" << groupId << endl;

	Buffer  buffer;
	RequestSetGroupStatus req;
	req.set_group_id(groupId);
	req.set_status(status);
	string str;
	req.SerializeToString(&str);
	
	VCCodec::PACKAGE_HEAD head;
	head.version	= GROUP_VERSION;
	head.length		= sizeof(head) + str.size();
	head.length		= htons(head.length);
	head.cmd		= CMD_SET_GROUP_STATUS;
	buffer.write((char*)&head, sizeof(head));
	buffer.write(str.data(), str.size());
	if(!Helper::Socket::write(g_fd, &buffer)){
		cout << "send fail" << endl;
	} else {
		cout << "send succ, begin to read" << endl;
	}

	Buffer inBuffer;
	readResp(&inBuffer);
	inBuffer.read((char*)&head, sizeof(head));

	string data(head.length, 0);
	inBuffer.read(data.data(), head.length);
	ResponseSetGroupStatus resp;
	resp.ParseFromString(data);
	cout << "set group status result:" << resp.result() << endl;
}
/*}}}*/

int connect(const char* addr, int port){/*{{{*/
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_addr.s_addr = inet_addr(addr);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);

	connect(fd, (struct sockaddr *)&sin, sizeof(sin));

	return fd;
}/*}}}*/

int main(int argc, char* argv[]){/*{{{*/
	g_fd = connect(SERVER_HOST, SERVER_PORT);
	printHelp();
	if(argc <= 1){
		string input;
		while(cin >> input){
			if(input.compare("help") == 0){
				printHelp();
			} else if(input.compare("create") == 0){
				createGroup();
			} else if(input.compare("get") == 0){
				getGroupInfo();
			} else if(input.compare("dismiss") == 0){
				dismissGroup();
			} else if(input.compare("join") == 0){
				joinGroup();
			} else if(input.compare("quit") == 0){
				quitGroup();
			} else if(input.compare("msg") == 0) {
				msgGroup();
			} else if(input.compare("register_chat") == 0) {
				registerChat();
			} else if(input.compare("recv_msg") == 0) {
				recvMsg();
			} else if(input.compare("set_group_status") == 0) {
				setGroupStatus();
			} else if(input.compare("broadcast") == 0) {
				broadcastGroup();
			} else {
				printHelp();
			}

			input = "";
			cout << endl;
		}
	} else {
		int cnt = atoi(argv[1]);
		for(int i = 0; i < cnt; ++i){
			getGroupInfoRepated();		
		}
	}
}/*}}}*/

