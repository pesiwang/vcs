#include "vcs/event_base/event_base.h"
#include "vcs/util/server_config.h"
#include "vcs/doctor/server_doctor.h"
#include "client_listener.h"
#include <string>

using namespace vcs;
using namespace std;

int main(int argc, char* argv[]) {
    if(argc != 2){
        fprintf(stderr, "Usage:%s <config.json>\n", argv[0]);
        return EXIT_FAILURE;
    }

    ServerConfig* cfg   =  Singleton<ServerConfig>::instance();
    cfg->load(argv[1]);

    string doctorHost   = cfg->getStrValue("doctor.host");
    int doctorPort      = cfg->getIntValue("doctor.port");
    ServerDoctor::instance()->init(doctorHost.c_str(), doctorPort);

    ClientListener* clientListener = Singleton<ClientListener>::instance();
    string clientHost   = cfg->getStrValue("network.client.host");
    int clientPort      = cfg->getIntValue("network.client.port");
    clientListener->fd  = Helper::Socket::listen(clientHost.c_str(), (unsigned short)clientPort);
    if(clientListener->fd < 0){
        log_error("start fail, client listener listen on %s:%d fail", clientHost.c_str(), clientPort);
        exit(EXIT_FAILURE);
    }
    EventBase::instance()->add(clientListener);
    log_info("client listener listen on %s:%d", clientHost.c_str(), clientPort);

    EventBase::instance()->dispatch();
}

