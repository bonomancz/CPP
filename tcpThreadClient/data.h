#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

class Datas{
private:
	string url;
	string host;

public:
	void trim(string& message);
	string getSendMessage();
	string getServerResponse(string& message);
	void setRemoteUrl(string& url);
	void setRemoteHost(string& host);
};
