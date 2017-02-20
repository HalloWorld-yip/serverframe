#include "LogFile.h"

#define WAIT_TIME	500
#pragma warning(disable:4996)
IOFile::IOFile(const char* filename)
	:fp_(::fopen(filename, "a+"))
{
	
}



IOFile::~IOFile()
{
	::fclose(fp_);
}

int IOFile::read(const char * buf, size_t len)
{

	return 0;
}

int IOFile::append(const char * str, size_t len)
{

	int wlen = write(str,len);
	len -= wlen;
	while (len)
	{
		size_t n = write(str, len);
		if (n == 0)
		{
			int err = ferror(fp_);
			if (err)
			{
				fprintf(stderr, " LogFile::append() failed %d\n", err);
			}
			break;
		}
		wlen += n;
		len -= n;
	}
	return 0;
}

int IOFile::write(const char * str, size_t len)
{
	return fwrite(str,1,len,fp_);
	
}



LogFile::LogFile()
	:m_log("level.log.txt"),m_cond(m_lock),
	running(false),curBuf(new Buffer),nextBuf(new Buffer)
{
}


void LogFile::append(const char * logline, size_t len)
{
	LockGuard guard(m_lock);
	if (curBuf->avail() < len)
	{
		buffers.push_back(std::move(curBuf));
		if (nextBuf)
		{
			curBuf = std::move(nextBuf);
		}
		else
		{
			curBuf.reset(new Buffer);
		}
		curBuf->append(logline, len);
		m_cond.notify();
	}
	else
		curBuf->append(logline, len);
}

unsigned LogFile::workthread()
{
	BufferPtr bufferPtr1(new Buffer);
	BufferPtr bufferPtr2(new Buffer);
	BufferVector bufferToWrite;
	while (running)
	{
		{
			LockGuard guard(m_lock);
			if (buffers.empty())
			{
				m_cond.waitForSeconds(WAIT_TIME);
			}
			buffers.push_back(std::move(curBuf));
			curBuf = std::move(bufferPtr1);
			bufferToWrite.swap(buffers);
			if (!nextBuf)
			{
				nextBuf = std::move(bufferPtr2);
			}
		}
		BufferPtr iterPtr;
		int sz = bufferToWrite.size();
		for (int i = 0; i < sz-2; i++)
		{
			iterPtr=std::move(bufferToWrite[i]);
			m_log.append(iterPtr->data(), iterPtr->len()); 
		}

		bufferPtr1 = std::move(bufferToWrite[sz - 1]);
		m_log.append(bufferPtr1->data(), bufferPtr1->len());
		bufferPtr1->clear();
		if (sz > 1)
		{
			iterPtr = std::move(bufferToWrite[sz - 2]);
			m_log.append(iterPtr->data(), iterPtr->len());
			if (!bufferPtr2)
			{
				bufferPtr2 = std::move(iterPtr);
				bufferPtr2->clear();
			}
			iterPtr.reset();
		}
		bufferToWrite.clear();
		
		

	}
	return 0;
}

 LogFile logFile;
