#ifndef _UTIL_VC_CODEC_H
#define _UTIL_VC_CODEC_H

#include "event_base/event_base.h"
#include <string>

class VCCodec {
	public:
		enum PACKAGE_STATUS {
			PACKAGE_STATUS_READY	= 0,
			PACKAGE_STATUS_PARTIAL,
			PACKAGE_STATUS_CORRUPT
		};

		#pragma pack(1)
		struct PACKAGE_HEAD {
			unsigned char	version;
			unsigned short	length;
			unsigned char	cmd;
		};
		#pragma pack()

		class Package {
			public:
				Package() : status(PACKAGE_STATUS_READY){}
				virtual ~Package(){}

				PACKAGE_STATUS status;
				PACKAGE_HEAD head;
				std::string body;
		};

	public:
		static Package* decode(vcs::Buffer*);
		static vcs::Buffer* encode(unsigned char version, unsigned char cmd, const std::string& body);

};


#endif

