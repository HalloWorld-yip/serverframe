#pragma once
#include<stdio.h>
#include<vector>
#include<memory>
#include<process.h>
#include"Lock.h"
#include"Buffer.h"
#include"Logger.h"

//处理文件输入输出
typedef	 std::unique_ptr<Buffer>	BufferPtr;
typedef  std::vector<BufferPtr> BufferVector;

class IOFile
{
public:

	IOFile(const char* filename);
	~IOFile();

	int read(const char* buf, size_t len);//暂时不需要
	int append(const char* str, size_t len);

private:
	int write(const char* str, size_t len);

	FILE *fp_;
};
class LogStream;


/*
既然作为背景线程，直接拥有一个自己的线程
*/



class LogFile
{
public:
	LogFile();
	//负责交换双缓冲，并将数据写入文件。
	void append(const char* logline, size_t len);
	unsigned workthread();
	void run() { running = true; }
	void stop() { running = false; }

private:
	IOFile	m_log;
	Lock	m_lock;
	Condition m_cond;
	bool	running;

	BufferPtr curBuf;
	BufferPtr nextBuf;
	BufferVector buffers;
	

};


extern LogFile logFile;
