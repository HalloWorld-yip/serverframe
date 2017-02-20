#include "Buffer.h"
#include<string.h>
#define BUFSIZE	(4096*1024)

Buffer::Buffer()
{
	m_data = new char[BUFSIZE];
	m_len = 0;
}

Buffer::~Buffer()
{
	delete m_data;
}

void Buffer::append(const char * str, size_t len)
{
	if (avail() > len)
	{
		memcpy(m_data + m_len, str, len);
		m_len += len;
	}
}

inline size_t Buffer::avail() const
{
	return BUFSIZE-m_len;
}
