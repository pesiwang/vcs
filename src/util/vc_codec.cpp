#include "util/vc_codec.h"
#include <arpa/inet.h>

using namespace std;
using namespace vcs;

VCCodec::Package* VCCodec::decode(Buffer* buffer){/*{{{*/
	Package* p = new Package();

	if(sizeof(p->head) != buffer->peek((char*)&(p->head), sizeof(p->head))){
		p->status = PACKAGE_STATUS_PARTIAL;
		return p;
	}
	p->head.length = ntohs(p->head.length);

	if(p->head.length < sizeof(p->head)){
		p->status = PACKAGE_STATUS_CORRUPT;
		return p;
	}

	if(buffer->size() < p->head.length){
		p->status = PACKAGE_STATUS_PARTIAL;
		return p;
	}

	buffer->shrink(sizeof(p->head));
	unsigned int bodyLength	= p->head.length - sizeof(p->head);

	string body(bodyLength, 0);
	if(buffer->read(body.data(), bodyLength) < bodyLength){
		p->status = PACKAGE_STATUS_CORRUPT;
		return p;
	}

	p->body		= body;
	p->status	= PACKAGE_STATUS_READY;

	return p;
}/*}}}*/

Buffer* VCCodec::encode(unsigned char version, unsigned char cmd, const string& body){/*{{{*/
	PACKAGE_HEAD head;
	head.version	= version;
	head.cmd		= cmd;
	
	head.length = sizeof(head) + body.size();
	head.length	= htons(head.length);

	Buffer* buffer	= new Buffer();
	buffer->write((char*)&head, sizeof(head));
	buffer->write(body.data(), body.size());

	return buffer;
}/*}}}*/

