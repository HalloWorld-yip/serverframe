#pragma once
#include<string>
#include"Timer.h"
#include"LogFile.h"
#include"Buffer.h"
using std::string;



/*
cach log in buffer,push to backend when LogStream is destroyed
hence LogStream is used as a tmp obj in a tmp Logger only
*/
class LogStream
{
public:

	LogStream& operator<<(const char* str);
	//LogStream& operator<<(const string& str);
	//LogStream& operator<<(const Buffer& c);

	LogStream& operator<<(unsigned short n);
	LogStream& operator<<(short n);
	LogStream& operator<<(unsigned n);
	LogStream& operator<<(int n);
	LogStream& operator<<(unsigned long c);
	LogStream& operator<<(long c);
	LogStream& operator<<(unsigned long long c);
	LogStream& operator<<(long long c);
	LogStream& operator<<(float f);
	LogStream& operator<<(double f);

	LogStream& operator<<(char c);
	~LogStream() ;
private:
	Buffer1<512> m_output;
};

//LogStream& operator<<(LogStream& out,const char* str);


/*
Logger :
1 devide level
2 format output
3 use a tmp LogStream to make sure a complete output
*/
class Logger
{
public:
	enum LogLevel
	{
		TRACE,
		DEBUG,
		INFO,
		WARN,
		WRONG,
		FATAL,
		NUM_LOG_LEVELS,
	};


	Logger(const char * sourcefile, int line, LogLevel level);
	static LogLevel initlevel() { return LogLevel::TRACE; }
	LogStream& stream() { return m_LogStream; }
	static LogLevel loglevel();
private:
	LogStream m_LogStream;
	Timer m_Timer;
};


extern Logger::LogLevel g_level;
inline Logger::LogLevel Logger::loglevel() { return g_level; }


#define TRACE if(Logger::loglevel()<=Logger::LogLevel::TRACE)Logger::Logger(__FILE__,__LINE__,Logger::LogLevel::TRACE).stream()
#define DEBUG if(Logger::loglevel()<=Logger::LogLevel::DEBUG)Logger::Logger(__FILE__,__LINE__,Logger::LogLevel::DEBUG).stream()
#define INFO if(Logger::loglevel()<=Logger::LogLevel::INFO)Logger::Logger(__FILE__,__LINE__,Logger::LogLevel::INFO).stream()
#define WARN if(Logger::loglevel()<=Logger::LogLevel::WARN)Logger::Logger(__FILE__,__LINE__,Logger::LogLevel::WARN).stream()
#define WRONG if(Logger::loglevel()<=Logger::LogLevel::WRONG)Logger::Logger(__FILE__,__LINE__,Logger::LogLevel::WRONG).stream()
#define FATAL if(Logger::loglevel()<=Logger::LogLevel::FATAL)Logger::Logger(__FILE__,__LINE__,Logger::LogLevel::FATAL).stream()
#define NUM_LOG_LEVELS if(Logger::loglevel()<=Logger::LogLevel::NUM_LOG_LEVELS)Logger::Logger(__FILE__,__LINE__,Logger::LogLevel::NUM_LOG_LEVELS).stream()
