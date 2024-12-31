#include <iostream>
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
	string strip(const string& inputString, string chars);
	vector<string> split(const string& inputString, char delimiter);
};

