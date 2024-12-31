#include "httpd.h"


void Httpd::start() {
	try {
		while (true) {
			try {
				SOCKET clientSocket;
				if ((clientSocket = sock.acceptSock()) != INVALID_SOCKET) {
					this->threads.startNewThread([this, clientSocket]() {
						this->handleClient(sock, clientSocket);
					});
					this->threads.removeFinishedThreads();
				}
				else {
					std::this_thread::sleep_for(std::chrono::milliseconds(100)); // busy waiting protection
				}
				//this->status();
			}
			catch (runtime_error& e) {
				cerr << "main(): Runtime error: " << e.what() << endl;
			}
		}
	}
	catch (runtime_error& e) {
		cerr << "main(): Runtime error: " << e.what() << endl;
	}
	catch (...) {
		cerr << "main(): Unknown error occured." << endl;
	}
}

void Httpd::initialize() {
	this->loadConfigParams();
	sock.setServerPort(stoi(this->config["listen_port"]));
	sock.build();
}

void Httpd::stop() {
	this->threads.removeAllThreads();
}


void Httpd::status() {
	//cout << "Threads in ThreadPool: " << to_string(this->threads.getThreadPoolSize()) << endl;
	cout << this->threads.getThreadPoolStatistic() << endl;
}


void Httpd::handleClient(Socket& sock, SOCKET clientSocket) {
	try {
		string clientMessage;
		auto callbackHandler = [this](SOCKET &clientSocket, string &clientMessage) {
			this->receivedMessageHandler(clientSocket, clientMessage);
		};
		sock.receiveSock(clientSocket, clientMessage, callbackHandler);
		closesocket(clientSocket);
	}
	catch (runtime_error& e) {
		cerr << "HandleClient(): Runtime error: " << e.what() << endl;
	}
	catch (...) {
		cerr << "HandleClient(): Unknown error occured." << endl;
	}
}


void Httpd::receivedMessageHandler(SOCKET &clientSocket, string &clientMessage) {
	try {
		string clientInfo = tm.getMillisecTime() + " Client: " + string(sock.getClientIpAddr(clientSocket));
		if (datas.isHttpRequest(clientMessage)) {
			string sendMessage = datas.getHttpReponse(404);
			if (datas.isExistingUrl(clientMessage)) {
				sendMessage = datas.getHttpReponse(200);
			}
			sock.sendSock(clientSocket, sendMessage);
			clientInfo += ", " + datas.getClientRequest(clientMessage);
			cout << clientInfo << endl;
		}
		// set thread as finished
		std::thread::id clientThreadId = std::this_thread::get_id();
		this->threads.setThreadFinished(clientThreadId);
	}
	catch (runtime_error& e) {
		cerr << "receivedMessageHandler(): Runtime error: " << e.what() << endl;
	}
	catch (...) {
		cerr << "receivedMessageHandler(): Unknown error occured." << endl;
	}
}

vector<string> Httpd::processArguments(int& arg_c, char**& arg_v) {
	string exceptMessage{ "Missing arguments\nUsage: http_server.exe --config=\"[path to config file]\"" };
	if (arg_c < 2) {
		throw runtime_error(exceptMessage);
	}
	string configFile = string(arg_v[1]);
	size_t configPosition = configFile.find("--config=");
	if (configPosition == string::npos) {
		throw runtime_error(exceptMessage);
	}
	vector<string> arguments;
	for (int i = 1; i < arg_c; i++) {
		string arg = arg_v[i];
		size_t pos = arg.find("=") + 1;
		arg = arg.substr(pos);
		arguments.push_back(arg);
	}
	return arguments;
}

void Httpd::setServerConfigFile(string& configFile) {
	this->configFileName = configFile;
}


void Httpd::loadConfigParams() {
	try {
		ifstream fin(this->configFileName);
		if (!fin.is_open()) {
			throw runtime_error("Unable to open config file " + this->configFileName);
		}
		stringstream buffer;
		buffer << fin.rdbuf();
		fin.close();
		string newLine, key, space, value;
		while (getline(buffer, newLine)) {
			if (newLine.find("#") != std::string::npos) {
				continue;
			}
			istringstream lineBuffer(newLine);
			lineBuffer >> key >> value;
			if (!datas.strip(key, " ").empty() && !datas.strip(value, " ").empty()) {
				this->config[datas.strip(key, " ")] = datas.strip(value, " ");
			}
		}
		
		cout << "Config values:" << endl;
		for (auto& pair : this->config) {
			cout << pair.first << " :: " << pair.second << endl;
		}
	}
	catch (runtime_error& e) {
		cerr << "loadConfigParams(): Exception: " << e.what() << endl;
	}
}