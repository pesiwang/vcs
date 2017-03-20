#include "vcs/event_base/event_base.h"
#include "vcs/util/server_config.h"
#include "vcs/worker/worker_manager.h"
#include "vcs/doctor/server_doctor.h"
#include "protobuf/group_package.pb.h"
#include "group_config.h"
#include "group_callback.h"
#include <string>

using namespace vcs;
using namespace std;

int main(int argc, char* argv[]){
	
	if(argc != 2){
		fprintf(stderr, "usage:%s <config.json>\n", argv[0]);
		return 0;
	}

	Singleton<ServerConfig>::instance()->load(argv[1]);
	ServerConfig* serverConfig = Singleton<ServerConfig>::instance();

	string doctorHost	= serverConfig->getStrValue("doctor.host");
	int doctorPort		= serverConfig->getIntValue("doctor.port");
	ServerDoctor::instance()->init(doctorHost.c_str(), doctorPort);

	GroupConfig* groupConfig = Singleton<GroupConfig>::instance();
	string serverHost = serverConfig->getStrValue("server.host");
	int serverPort	= serverConfig->getIntValue("server.port");
	int taskQueueSizeThreshold		= serverConfig->getIntValue("worker.task_queue_size_threshold");
	int outcomeQueueSizeThreshold	= serverConfig->getIntValue("worker.outcome_queue_size_threshold");
	WorkerManager::instance()->init(serverHost.c_str(), serverPort, 
			groupConfig, taskQueueSizeThreshold, outcomeQueueSizeThreshold);
	WorkerManager::instance()->addClientCallback(Singleton<ChatClientCallback>::instance());

	GOOGLE_PROTOBUF_VERIFY_VERSION;
	EventBase::instance()->dispatch();
	google::protobuf::ShutdownProtobufLibrary();

	return 0;
}

