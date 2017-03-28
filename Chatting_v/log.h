#pragma once
#pragma warning(disable:4996)
#include <fstream>
#include <iostream>
#include <ctime>
using std::fstream;
using std::endl;
using std::cout;
class log
{
private:
	log()   //构造函数是私有的  
	{
		fout.open("log.txt",std::ios::out|std::ios::app);
		cout << fout.is_open() << endl;
	}
	static log *m_pInstance;
	fstream fout;
	struct tm* local;
	time_t t;
public:
	static log* GetInstance()
	{
		if (m_pInstance == NULL)  //判断是否第一次调用  
			m_pInstance = new log();
		return m_pInstance;
	}
	~log() 
	{
		fout.close();
		delete m_pInstance;
	}
	void log_error(const char* text,const int code=0) {
		t = time(NULL); 
		local = localtime(&t);
		fout << local->tm_mon<<'/'<<local->tm_mday<<" "<<local->tm_hour<< ":" <<
			local->tm_min << ":" << local->tm_sec << " " << text<<" : "<<code<<endl; }
};




void Log(const char* text, int code = 0);