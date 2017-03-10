#include "doctor/doctor_codec.h"
#include <string>
#include "event_base/event_base.h"

using namespace std;
using namespace vcs;

DoctorCodec::Request* DoctorCodec::decode(vcs::Buffer* buffer){/*{{{*/
	size_t size = buffer->size();
	if(size <= 0){
		return NULL;
	}

	char* data = new char[size];
	int len = (int)(buffer->peek(data, size));

	bool found = false;
	int requestLen = 0;
	for(int i = 0; i < len; ++i){
		if(data[i] == '\r'){
			if(i + 1 >= len){
				break;
			}

			if(data[i+1] == '\n'){
				found = true;
				requestLen = i + 1 + 1;
				break;
			}
		}
	}

	if( ! found){
		log_debug("cmd end str not found");
		delete data;
		return NULL;
	}

	buffer->shrink(requestLen);
	string s = string(data, 0, requestLen);

	delete data;
	return DoctorCodec::_parseRequest(s);
}/*}}}*/

DoctorCodec::Request* DoctorCodec::_parseRequest(const string& str){/*{{{*/
	DoctorCodec::Request* request = new DoctorCodec::Request();
	size_t startIndex = 0;
	string element;
	size_t pos;
	size_t validLen = str.size() - 2;
	while(startIndex < validLen){
		pos = str.find(" ", startIndex);
		if(pos != string::npos){
			if(pos > startIndex){
				element = str.substr(startIndex, pos - startIndex);
				request->abilityName = element;
				log_debug("doctor cmd is %s", request->abilityName.c_str());
				startIndex = pos + 1;
				break;
			} else {
				startIndex = pos + 1;
			}
		} else {
			if(validLen > startIndex){
				request->abilityName = str.substr(startIndex, validLen);
				log_debug("doctor cmd is %s", request->abilityName.c_str());
				return request;
			} else {
				log_error("no doctor cmd");
				return NULL;
			}
		}
	}

	while(startIndex < validLen) {
		pos = str.find(" ", startIndex);
		if(pos != string::npos){
			if(pos > startIndex){
				element = str.substr(startIndex, pos - startIndex);
				request->paramVector.push_back(element);
				log_debug("doctor param %s", element.c_str());
			}
			startIndex = pos + 1;
		} else {
			if(validLen > startIndex){
				element = str.substr(startIndex, validLen - startIndex);
				request->paramVector.push_back(element);
				log_debug("doctor param %s", element.c_str());
			}
			break;
		}
	}

	return request;
}/*}}}*/

vcs::Buffer* DoctorCodec::encode(string data){/*{{{*/
	data.append("\r\n");
	Buffer* buffer = new Buffer();
	buffer->write(data.data(), data.size());

	return buffer;
}/*}}}*/

