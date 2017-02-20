#pragma once
#include<stdio.h>
#include<vector>
#include<memory>
#include<process.h>
#include"Lock.h"
#include"Buffer.h"
#include"Logger.h"

//�����ļ��������
typedef	 std::unique_ptr<Buffer>	BufferPtr;
typedef  std::vector<BufferPtr> BufferVector;

class IOFile
{
public:

	IOFile(const char* filename);
	~IOFile();

	int read(const char* buf, size_t len);//��ʱ����Ҫ
	int append(const char* str, size_t len);

private:
	int write(const char* str, size_t len);

	FILE *fp_;
};
class LogStream;


/*
��Ȼ��Ϊ�����̣߳�ֱ��ӵ��һ���Լ����߳�
*/



class LogFile
{
public:
	LogFile();
	//���𽻻�˫���壬��������д���ļ���
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
