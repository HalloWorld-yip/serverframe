#pragma once


class Buffer
{
public:
	Buffer();
	~Buffer();

	void clear() { m_len = 0; }
	void append(const char* str, size_t len);

	char* data()const { return m_data; }
	size_t len()const { return m_len; }
	size_t avail()const;

private:
	size_t m_len;
	char *m_data;
};

template<int sz=512>
class Buffer1
{
public:
	Buffer1();
	~Buffer1();

	void clear() { m_len = 0; }
	void append(const char* str, size_t len);

	char* data()const { return m_data; }
	size_t len()const { return m_len; }
	size_t avail()const;

private:
	size_t m_len;
	char *m_data;
};

template<int sz>
Buffer1<sz>::Buffer1()
{
	m_data = new char[sz];
	m_len = 0;
}


template<int sz>
Buffer1<sz>::~Buffer1()
{
	delete m_data;
}

template<int sz>
void Buffer1<sz>::append(const char * str, size_t len)
{
	if (avail() > len)
	{
		memcpy(m_data + m_len, str, len);
		m_len += len;
	}
}

template<int sz>
inline size_t Buffer1<sz>::avail() const
{
	return sz - m_len;
}
