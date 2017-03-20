#include "vcs/event_base/event_base.h"
#include "model/group_storage.h"
#include "hiredis/hiredis.h"
#include <string>

using namespace std;

GroupStorage::GroupStorage() : _context(NULL) {

}

GroupStorage::~GroupStorage(){
	if(this->_context){
		redisFree(this->_context);
		this->_context = NULL;
	}
}

bool GroupStorage::connect(){/*{{{*/
	if(this->_context != NULL){
		redisFree(this->_context);
		this->_context = NULL;
	}

	struct timeval timeout = { 1, 500000 }; // 1.5 seconds	
	this->_context	= redisConnectWithTimeout(this->host.c_str(), this->port, timeout);
	if(this->_context == NULL || this->_context->err) {
		if(this->_context){
			log_error("Connection error: %s\n", this->_context->errstr);
		} else {
			log_error("Connection error: can not allocate redis context");	
		}

		return false;
	}

    if (!auth()) {
        return false;
    }

	redisSetTimeout(this->_context, timeout);
	return true;
}/*}}}*/

bool GroupStorage::auth() {/*{{{*/
    redisReply *reply = static_cast<redisReply *>(redisCommand(this->_context, "AUTH next2015"));
    if (this->_context->err != 0 || !reply) {
        log_error("AUTH failed:%s\n", this->_context->errstr);
        if (reply) {
            freeReplyObject(reply);
        }

        return false;
    }

    bool succ = false;
    switch(reply->type) {
        case REDIS_REPLY_STATUS:
            succ = true;
            break;

        default:
            succ = false;
            log_error("AUTH  failed:%s", this->_context->errstr);
    }

    freeReplyObject(reply);

    return succ;
}/*}}}*/

bool GroupStorage::isConnected(){/*{{{*/
	redisReply* reply = (redisReply*)redisCommand(this->_context, "PING");	
	if(this->_context->err != 0){
		log_error("PING fail: %s", this->_context->errstr);
		if(reply){
			freeReplyObject(reply);
		}
		return false;
	}

	bool succ = false;
	switch(reply->type){
		case REDIS_REPLY_STATUS:
			succ = true;
			break;

		default:
			succ = false;
			log_error("PING fail: %s", reply->str);	
	}

	freeReplyObject(reply);

	return succ;
}/*}}}*/

bool GroupStorage::get(const string& hashKey, const string& groupId, string& content) {/*{{{*/
	content = "";
	if(groupId.size() <= 0){
		log_error("groupId is empty");
		return false;	
	}
	
	redisReply* reply = (redisReply*)redisCommand(this->_context, "HGET %s %s", hashKey.c_str(), groupId.c_str());	
	if(this->_context->err != 0){
		log_error("HGET %s %s fail: %s", hashKey.c_str(), groupId.c_str(), this->_context->errstr);
		if(reply){
			freeReplyObject(reply);
		}
		return false;
	}

	bool succ = false;
	switch(reply->type){
		case REDIS_REPLY_STRING:
			succ = true;
			content = reply->str;
			break;

		case REDIS_REPLY_NIL:
			succ = true;
			content = "";
			break;

		default:
			succ = false;
			log_error("HGET %s %s fail: %s", hashKey.c_str(), groupId.c_str(), reply->str);	
	}

	freeReplyObject(reply);

	return succ;
}/*}}}*/

bool GroupStorage::set(const string& hashKey, const string& groupId, const string& content) {/*{{{*/
	if(groupId.size() <= 0){
		log_error("groupId is empty");
		return false;	
	}
	
	if(content.size() <= 0){
		log_error("content is empty");
		return false;	
	}
	
	redisReply* reply = (redisReply*)redisCommand(this->_context, "HSET %s %s %s", hashKey.c_str(), groupId.c_str(), content.c_str());	
	if(this->_context->err != 0){
		log_error("HSET %s %s fail: %s", hashKey.c_str(), groupId.c_str(), this->_context->errstr);
		if(reply){
			freeReplyObject(reply);
		}
		return false;
	}

	bool succ = false;
	switch(reply->type){
		case REDIS_REPLY_INTEGER:
			succ = true;
			break;

		default:
			succ = false;
			log_error("HSET %s %s fail: %s", hashKey.c_str(), groupId.c_str(), reply->str);	
	}

	freeReplyObject(reply);

	return succ;
}/*}}}*/

bool GroupStorage::del(const string& hashKey, const string& groupId) {/*{{{*/
	if(groupId.size() <= 0){
		log_error("groupId is empty");
		return false;	
	}
	
	redisReply* reply = (redisReply*)redisCommand(this->_context, "HDEL %s %s", hashKey.c_str(), groupId.c_str());	
	if(this->_context->err != 0){
		log_error("HDEL %s %s fail: %s", hashKey.c_str(), groupId.c_str(), this->_context->errstr);
		if(reply){
			freeReplyObject(reply);
		}
		return false;
	}

	bool succ = false;
	switch(reply->type){
		case REDIS_REPLY_INTEGER:
			succ = true;
			break;

		default:
			succ = false;
			log_error("HDEL %s %s fail: %s", hashKey.c_str(), groupId.c_str(), reply->str);	
	}

	freeReplyObject(reply);

	return succ;
}/*}}}*/

