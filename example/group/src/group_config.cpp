#include "vcs/util/server_config.h"
#include "group_config.h"
#include "group_router.h"
#include "group_codec.h"
#include "group_service.h"

using namespace vcs;

WorkerRouter* GroupConfig::instanceRouter(){
	return Singleton<GroupRouter>::instance();
}

WorkerCodec* GroupConfig::instanceCodec(){
	return Singleton<GroupCodec>::instance();
}

WorkerService* GroupConfig::createService(){
	return new GroupService();
}

int GroupConfig::threadNum(){
	return Singleton<ServerConfig>::instance()->getIntValue("worker.thread_num");
}

int GroupConfig::clientMaxIdleTime(){
	return Singleton<ServerConfig>::instance()->getIntValue("worker.client_max_idle_time");
}

