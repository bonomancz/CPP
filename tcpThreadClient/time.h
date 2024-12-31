#pragma once
#include <iostream>
#include <chrono>
#include <string>
#include <iomanip>
#include <sstream>

using namespace std;
using namespace std::chrono;

class Time{
private:
public:
	std::string getMillisecTime();
	std::string getTimeFromTimestamp(long int timeStamp);
	unsigned int getTimeStamp();
};

