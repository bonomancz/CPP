#include <string>
#include <sstream>
#include <fstream>
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>
#include <unordered_map>
#include "socket.h"
#include "data.h"
#include "threads.h"
#include "time.h"
#include "file.h"

#pragma once

using namespace std;

class Httpd{
private:
	Socket sock;
	Data datas;
	Threads threads;
	Time tm;
	File fl;
	string configFileName;
	unordered_map<string, string> config;

public:
	void initialize();
	void start();
	void stop();
	void status();
	void handleClient(Socket& sock, SOCKET clientSocket);
	void receivedMessageHandler(SOCKET& clientSocket, string& clientMessage);
	vector<string> processArguments(int& arg_c, char**& arg_v);
	void setServerConfigFile(string& configFile);
	void loadConfigParams();
};


