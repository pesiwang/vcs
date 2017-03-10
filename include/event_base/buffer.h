#ifndef _VCS_BUFFER_H
#define _VCS_BUFFER_H

#include <string.h>

namespace vcs
{
	class Buffer
	{
	public:
		Buffer(size_t capacity = 1024);
		~Buffer();
		size_t size() const { return _size; }
		size_t capacity() const { return _capacity; }

		size_t shrink(size_t count);
		size_t peek(char* data, size_t count) const;
		size_t read(const char* data, size_t count);
		void write(const char* data, size_t count);
		void append(const Buffer* buffer);

		void shrinkByte();
		char peekByte() const;
		char readByte();
		void writeByte(const char data);

		void shrinkUnsignedByte();
		unsigned char peekUnsignedByte() const;
		unsigned char readUnsignedByte();
		void writeUnsignedByte(const unsigned char data);

		void shrinkShort();
		short peekShort() const;
		short readShort();
		void writeShort(const short data);

		void shrinkUnsignedShort();
		unsigned short peekUnsignedShort() const;
		unsigned short readUnsignedShort();
		void writeUnsignedShort(const unsigned short data);

		void shrinkInt();
		int peekInt() const;
		int readInt();
		void writeInt(const int data);

		void shrinkUnsignedInt();
		unsigned int peekUnsignedInt() const;
		unsigned int readUnsignedInt();
		void writeUnsignedInt(const unsigned int data);

	private:
		void _inflate();

		size_t _size;
		size_t _capacity;

		char *_cursor_beg_ptr, *_cursor_end_ptr;
		char *_data_beg_ptr, *_data_end_ptr;
	};
}

#endif
