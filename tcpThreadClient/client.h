#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "socket.h"
#include "data.h"
#include "time.h"
#include "threads.h"
#include "openssl.h"

using namespace std;

class Client{
private:
	Socket socket;
	Datas dat;
	Time tm;
	Threads threads;
	int threadsCount;
	int remotePort;
	string remoteHost;
	string remoteUrl;
	condition_variable cvStop, cvStart;
	mutex mtxStart, mtxStop, mtxLog;
public:
	void run();
	void runInThreads();
	void sayHello(string& message);
	void setHost(string& host);
	void setPort(int& port);
	void setUrl(string& url);
	void setThreadsCount(int& threadsCount);
	int getThreadsCount();
};

