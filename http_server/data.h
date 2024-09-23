#include <string>
#include <list>
#include <regex>
#include <sstream>

#pragma once

using namespace std;

class Data{
private:

public:
	string getHttpReponse(int httpResultCode);
	bool isHttpRequest(string &message);
	bool isExistingUrl(string &message);
	string getRequestUrl(string &message);
	string getClientRequest(string& message);
};

