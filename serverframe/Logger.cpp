#include "Logger.h"



LogStream & LogStream::operator<<(const char * str)
{
	m_output.append(str, strlen(str));
	return *this;
}
LogStream & LogStream::operator<<(unsigned short n)
{
	char  num[32];
	size_t sz = snprintf(num, 32, "%hu", n);
	m_output.append(num, sz);
	return *this;
}
LogStream & LogStream::operator<<(short n)
{
	char  num[32];
	size_t sz = snprintf(num, 32, "%hd", n);
	m_output.append(num, sz);
	return *this;
}
LogStream & LogStream::operator<<(unsigned n)
{
	char  num[32];
	size_t sz = snprintf(num, 32, "%u", n);
	m_output.append(num, sz);
	return *this;
}
LogStream & LogStream::operator<<(int n)
{
	char  num[33];
	size_t sz = snprintf(num, 33, "%d", n);
	m_output.append(num, sz);
	return *this;
}
LogStream & LogStream::operator<<(unsigned long n)
{
	char  num[65];
	size_t sz = snprintf(num, 65, "%lu", n);
	m_output.append(num, sz);
	return *this;
}
LogStream & LogStream::operator<<(long n)
{
	char  num[65];
	size_t sz = snprintf(num, 65, "%ld", n);
	m_output.append(num, sz);
	return *this;
}
LogStream & LogStream::operator<<(unsigned long long n)
{
	char  num[65];
	size_t sz = snprintf(num, 65, "%I64u", n);
	m_output.append(num, sz);
	return *this;
}
LogStream & LogStream::operator<<(long long n)
{
	char  num[65];
	size_t sz = snprintf(num, 65, "%I64d", n);
	m_output.append(num, sz);
	return *this;
}
LogStream & LogStream::operator<<(float n)
{
	*this << static_cast<double>(n);
	return *this;
}
LogStream & LogStream::operator<<(double n)
{
	char  num[32];
	size_t sz = snprintf(num, 65, "%.12g", n);
	m_output.append(num, sz);
	return *this;
}
LogStream & LogStream::operator<<(char c)
{
	m_output.append(&c, 1);
	return *this;
}
LogStream::~LogStream()
{
	m_output.append("\n", 1);
	logFile.append(m_output.data(), m_output.len()); 
}
//
//LogStream & operator<<(LogStream & out, const char * str)
//{
//	return out.operator<<(str);
//}



Logger::LogLevel g_level = Logger::initlevel();

Logger::Logger(const char * sourcefile, int line, LogLevel level)
{
	SYSTEMTIME t(m_Timer.get_time());
	char format[256];
	snprintf(format, 256, "%02d-%02d %02d:%02d:%02d %03d %s %d %d ", t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds, sourcefile, line, level);
	m_LogStream << format;
}