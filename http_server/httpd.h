#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>
#include "socket.h"
#include "data.h"
#include "threads.h"
#include "time.h"

#pragma once

using namespace std;

class Httpd{
private:
	Socket sock;
	Data datas;
	Threads threads;
	Time tm;

public:
	void start();
	void stop();
	void status();
	void handleClient(Socket& sock, SOCKET clientSocket);
	void receivedMessageHandler(SOCKET& clientSocket, string& clientMessage);
};


