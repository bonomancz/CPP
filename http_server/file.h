#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

class File{
private:
	string fileName;
	ifstream file;
public:
	bool open();
	bool close();
};

