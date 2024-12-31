#include <iostream>
#include <string>
#include <thread>
#include <iostream>
#include <iostream>
#include <string>
#include "client.h"

using namespace std;

bool isInteger(const std::string& input);

int main(int argc, char** argv) {
	try {
		if (argc < 6 || !isInteger(argv[2]) || !isInteger(argv[4]) || !isInteger(argv[5])) {
			throw runtime_error("Missing or bad arguments.\nUsage: tcpthreadclient [host (string)] [port (integer)] [path (string)] [threads count (integer)] [sleep ms (integer)]\n");
		}
		string host = string(argv[1]);
		string url = string(argv[3]);
		int port = stoi(argv[2]);
		int threadsCount = stoi(argv[4]);
		int sleepTime = stoi(argv[5]);
		Client cli;
		cli.setThreadsCount(threadsCount);
		cli.setHost(host);
		cli.setPort(port);
		cli.setUrl(url);

		while (true) {
			cli.runInThreads();
			std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
		}

	}
	catch (runtime_error& e) {
		cout << e.what() << endl;
	}
	return 0;
}


bool isInteger(const std::string& input) {
	try {
		size_t pos;
		int st = std::stoi(input, &pos);
		return pos == input.size();
	}
	catch (std::exception &e) {
		cerr << "Invalid argument: " << e.what() << endl;
		return false;
	}
}