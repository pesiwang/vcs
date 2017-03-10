

#ifndef _DOCTOR_CODEC_H
#define _DOCTOR_CODEC_H

#include <string>
#include <vector>
#include "event_base/event_base.h"

class DoctorCodec{
	public:
		class Request {
			public:
				std::string abilityName;
				std::vector<std::string> paramVector;
		};

	public:
		static DoctorCodec::Request* decode(vcs::Buffer*);
		static vcs::Buffer* encode(std::string data);
	
	private:
		static DoctorCodec::Request* _parseRequest(const std::string& str);
};
#endif

